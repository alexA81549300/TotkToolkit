#include <TotkToolkit/IO/Filesystem.h>

#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/DumpDir.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/WriteDir.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/Romfs.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/WriteDir.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/Threading/Tasks/IO/Filesystem/MountArchives.h>
#include <TotkToolkit/IO/PHYSFSCalls/Mount.h>
#include <TotkToolkit/IO/PHYSFSCalls/MountMemory.h>
#include <TotkToolkit/IO/PHYSFSCalls/MountHandle.h>
#include <TotkToolkit/IO/PHYSFSCalls/Unmount.h>
#include <TotkToolkit/IO/PHYSFSCalls/Float.h>
#include <TotkToolkit/IO/PHYSFSCalls/SetWriteDir.h>
#include <TotkToolkit/IO/PHYSFSCall.h>
#include <TotkToolkit/IO/Streams/Physfs/Physfs.h>
#include <TotkToolkit/IO/Streams/Multi/Multi.h>
#include <TotkToolkit/IO/Streams/File/File.h>
#include <Formats/Resources/ZSTD/ZSTDBackend.h>
#include <archiver_sarc.h>
#include <zstd_io.h>
#include <physfs.h>
#include <filesystem>
#include <map>
#include <algorithm>
#include <vector>
#include <regex>
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <sstream>
#include <cstring>

struct PHYSFSCallQueueEntry {
	std::vector<std::shared_ptr<TotkToolkit::IO::PHYSFSCall>> mCalls;
	PHYSFS_Context mContext;
	bool mIsClaimed;
};
std::map<std::thread::id, PHYSFSCallQueueEntry> sPHYSFSCallQueue;
std::shared_mutex sPHYSFSCallQueueMutex;
std::vector<std::shared_ptr<TotkToolkit::IO::PHYSFSCall>> sEntirePHYSFSCallQueue; // Used to get new threads up-to-date
std::shared_mutex sEntirePHYSFSCallQueueMutex;
void AddPHYSFSCall(std::shared_ptr<TotkToolkit::IO::PHYSFSCall> call) {
	{
		std::unique_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
		for (auto& it : sPHYSFSCallQueue) {
			it.second.mCalls.push_back(call);
		}
	}
	{
		std::unique_lock<std::shared_mutex> lock(sEntirePHYSFSCallQueueMutex);
		sEntirePHYSFSCallQueue.push_back(call);
	}
}
void ClaimBestPHYSFSContext() {
	std::unique_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
	std::thread::id bestThread = std::this_thread::get_id();
	F_U32 bestThreadCallCount = sPHYSFSCallQueue.contains(bestThread) ? sPHYSFSCallQueue.at(bestThread).mCalls.size() : std::numeric_limits<F_U32>::max();
	for (auto& it : sPHYSFSCallQueue) {
		if (!it.second.mIsClaimed) {
			if (it.second.mCalls.size() < bestThreadCallCount) {
				bestThread = it.first;
				bestThreadCallCount = it.second.mCalls.size();
			}
		}
	}

	// This context is the best one, but we've previously abandoned it.
	if (bestThread == std::this_thread::get_id() && sPHYSFSCallQueue.contains(bestThread) && !sPHYSFSCallQueue.at(bestThread).mIsClaimed) {
		sPHYSFSCallQueue.at(bestThread).mIsClaimed = true;
	}
	else if (bestThread != std::this_thread::get_id()) {
		// De-init this context if it exists. It's being thrown away.
		if (sPHYSFSCallQueue.contains(std::this_thread::get_id()))
			PHYSFS_deinitContext(sPHYSFSCallQueue[std::this_thread::get_id()].mContext);

		// Set the new context.
		sPHYSFSCallQueue[std::this_thread::get_id()] = sPHYSFSCallQueue.at(bestThread);

		// Mark the context as claimed.
		sPHYSFSCallQueue[std::this_thread::get_id()].mIsClaimed = true;

		// Erase the new context from where it previously was.
		sPHYSFSCallQueue.erase(bestThread);
			
		// Bind the new context.
		PHYSFS_bindContext(sPHYSFSCallQueue[std::this_thread::get_id()].mContext);
	}
}
void UnclaimPHYSFSContext() {
	std::unique_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
	sPHYSFSCallQueue.at(std::this_thread::get_id()).mIsClaimed = false;
}
void ExecutePHYSFSCallQueue() {
	std::shared_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
	for (std::shared_ptr<TotkToolkit::IO::PHYSFSCall> call : sPHYSFSCallQueue.at(std::this_thread::get_id()).mCalls) {
		call->Execute();
	}

	sPHYSFSCallQueue.at(std::this_thread::get_id()).mCalls.clear();
}

void BindCurrentThreadContext() {
	{
		std::shared_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
		if (sPHYSFSCallQueue.contains(std::this_thread::get_id())) {
			PHYSFS_bindContext(sPHYSFSCallQueue.at(std::this_thread::get_id()).mContext);
			return;
		}
	}

		PHYSFS_Context context = PHYSFS_allocContext();
		PHYSFS_initContext(context, "");
		PHYSFS_bindContext(context);
		PHYSFS_permitSymbolicLinks(true); // We trust our user. Plus, it helps with performance by eliminating checks in PHYSFS
		PHYSFS_permitDanglingWriteHandles(true); // Necessary for writing to multiple write dirs.
		PHYSFS_registerArchiver(&archiver_sarc_default);
	{
		std::unique_lock<std::shared_mutex> queueLock(sPHYSFSCallQueueMutex);
		std::shared_lock<std::shared_mutex> entireQueueLock(sEntirePHYSFSCallQueueMutex);

		PHYSFSCallQueueEntry entry;
		entry.mCalls = sEntirePHYSFSCallQueue;
		entry.mIsClaimed = true; // Default is true, should be set to false when the thread is done with it.
		entry.mContext = context;
		sPHYSFSCallQueue.insert({std::this_thread::get_id(), entry });
	}
}

struct SearchFilenamesByRegexCallbackData {
	SearchFilenamesByRegexCallbackData(std::string regex, std::shared_ptr<std::atomic<bool>> continueCondition) : mRegex(regex), mContinueCondition(continueCondition), mRetPaths() {

	}

	std::string mRegex;
	std::shared_ptr<std::atomic<bool>> mContinueCondition;
	std::vector<std::string> mRetPaths;
};

PHYSFS_EnumerateCallbackResult searchFilenamesByRegexCallback(void *data, const char *origdir, const char *fname) {
	SearchFilenamesByRegexCallbackData* callbackData = reinterpret_cast<SearchFilenamesByRegexCallbackData*>(data);
	
	if (!*callbackData->mContinueCondition)
		return PHYSFS_ENUM_STOP;
	
	std::string filePath = (std::filesystem::path(origdir) / std::filesystem::path(fname)).generic_string().c_str();

	std::regex regex(callbackData->mRegex.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);

	if (std::regex_search(fname, regex)) {
		callbackData->mRetPaths.push_back(filePath);
	}

	if (filePath.find_first_of('.') == std::string::npos) {
	// PHYSFS_isDirectory is incredibly slow...
	//if (PHYSFS_isDirectory(filePath.c_str())) {
		PHYSFS_enumerate(filePath.c_str(), searchFilenamesByRegexCallback, callbackData);
	}

	return PHYSFS_EnumerateCallbackResult::PHYSFS_ENUM_OK;
}

struct SearchFilenamesByExtensionCallbackData {
	SearchFilenamesByExtensionCallbackData(std::string extension, std::shared_ptr<std::atomic<bool>> continueCondition) : mExtension(extension), mContinueCondition(continueCondition), mRetPaths() {

	}

	std::string mExtension;
	std::shared_ptr<std::atomic<bool>> mContinueCondition;
	std::vector<std::string> mRetPaths;
};

PHYSFS_EnumerateCallbackResult searchFilenamesByExtensionCallback(void *data, const char *origdir, const char *fname) {
	SearchFilenamesByExtensionCallbackData* callbackData = reinterpret_cast<SearchFilenamesByExtensionCallbackData*>(data);

	if (!*callbackData->mContinueCondition)
		return PHYSFS_ENUM_STOP;

	// This is super slow:
	//std::string filePath = (std::filesystem::path(origdir) / std::filesystem::path(fname)).generic_string().c_str();
	// Instead we do this:
	std::string filePath = std::string(origdir) + "/" + fname;


	if (std::string(fname).ends_with(callbackData->mExtension))
		callbackData->mRetPaths.push_back(filePath);

	if (strchr(fname, '.') == 0) {
	// PHYSFS_isDirectory is incredibly slow...
	//if (PHYSFS_isDirectory(filePath.c_str())) {
		PHYSFS_enumerate(filePath.c_str(), searchFilenamesByExtensionCallback, callbackData);

		if (!*callbackData->mContinueCondition)
			return PHYSFS_ENUM_STOP;
	}

	return PHYSFS_EnumerateCallbackResult::PHYSFS_ENUM_OK;
}


namespace TotkToolkit::IO {
	_Filesystem Filesystem;

	void _Filesystem::Init() {
		PHYSFS_init("");
		TotkToolkit::Messaging::NoticeBoard::AddReceiver(this);
	}

	void _Filesystem::InitThread() {
		ClaimBestPHYSFSContext(); // See if there's a leftover context to start from
		BindCurrentThreadContext();
	}
	void _Filesystem::DeinitThread() {
		UnclaimPHYSFSContext(); // Make this PHYSFS context avaliable to other threads.
	}

	void _Filesystem::SyncThread() {
		ClaimBestPHYSFSContext(); // Is there an unused PHYSFS context that's more up-to-date?
		ExecutePHYSFSCallQueue(); // Go ahead and make sure things are up-to-date.
	}

	void _Filesystem::Mount(std::string path, std::string mountPoint, bool notifyFileChange, bool deferredFloating) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::Mount>(path, mountPoint, false, deferredFloating ? std::vector<std::string>() : mFloatDirs, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void _Filesystem::MountStream(std::shared_ptr<Formats::IO::Stream> stream, std::string filename, std::string mountPoint, bool notifyFileChange, bool deferredFloating) {
		std::shared_ptr<F_U8[]> buffer = stream->GetBuffer();
		F_UT bufferLength = stream->GetBufferLength();
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::MountMemory>(buffer, bufferLength, nullptr, filename, mountPoint, true, deferredFloating ? std::vector<std::string>() : mFloatDirs, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void _Filesystem::MountHandle(void* handle, std::string filename, std::string mountPoint, bool notifyFileChange, bool deferredFloating) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::MountHandle>(handle, mountPoint, filename, true, deferredFloating ? std::vector<std::string>() : mFloatDirs, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void _Filesystem::Unmount(std::string path, bool notifyFileChange) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::Unmount>(path, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void _Filesystem::Float() {
		std::shared_lock<std::shared_mutex> floatDirsLock(mFloatDirsMutex);
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::Float>(mFloatDirs));
		ExecutePHYSFSCallQueue();
	}
	// Be very careful with this! Changes must be undone before finishing thread work!
	bool _Filesystem::TempMount(std::string path, std::string mountPoint) {
		return PHYSFS_mount(path.c_str(), mountPoint.c_str(), 0);
	}
	// Be very careful with this! Changes must be undone before finishing thread work!
	bool _Filesystem::TempUnmount(std::string path) {
		return PHYSFS_unmount(path.c_str());
	}
	std::string _Filesystem::GetMountPoint(std::string path) {
		const char* mountPoint = PHYSFS_getMountPoint(path.c_str());
		if (mountPoint != nullptr)
			return std::string(mountPoint);
		else
			return "";
	}
	std::string _Filesystem::GetDumpDir() {
		std::shared_lock<std::shared_mutex> lock(mDumpDirMutex);
		return mDumpDir;
	}
	void _Filesystem::SetDumpDir(std::string dir) {
		std::unique_lock<std::shared_mutex> lock(mDumpDirMutex);
		dir = std::filesystem::path(dir).generic_string(); // Make standard
		mDumpDir = dir;
	}
	std::string _Filesystem::GetWriteDir() {
		std::shared_lock<std::shared_mutex> lock(mWriteDirMutex);
		return mWriteDir;
	}
	void _Filesystem::SetWriteDir(std::string dir) {
		std::unique_lock<std::shared_mutex> lock(mWriteDirMutex);
		dir = std::filesystem::path(dir).generic_string(); // Make standard
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::SetWriteDir>(dir));
		ExecutePHYSFSCallQueue();
		mWriteDir = dir;
	}

	bool _Filesystem::FileExists(std::string filepath) {
		PHYSFS_Stat stat;
		return PHYSFS_stat(filepath.c_str(), &stat) != 0;
	}
	// TOTKTOOLKIT_TODO_FUNCTIONAL: Implement ZSTD on a physfs level to save hella memory
	std::map<std::string, std::shared_ptr<Formats::IO::Stream>> packZstdStreamCache;
	std::shared_ptr<Formats::IO::Stream> _Filesystem::OpenReadStream(std::string filepath) {
		std::shared_ptr<Formats::IO::Stream> res;
		
		PHYSFS_File* file = PHYSFS_openRead(filepath.c_str());
		if (file == nullptr && (file = PHYSFS_openRead(std::filesystem::path(filepath).replace_extension(std::string("b") + std::filesystem::path(filepath).extension().string().substr(1)).string().c_str())) == nullptr) {
			res = nullptr;
		}
		else {
			std::shared_ptr<TotkToolkit::IO::Streams::Physfs::Physfs> stream = std::make_shared<TotkToolkit::IO::Streams::Physfs::Physfs>(file, false);

			if (stream != nullptr && filepath.ends_with(".zs")) {
				// TOTKTOOLKIT_TODO_FUNCTIONAL: Check for zstandard compression signature. // NAH do it all at physfs actually
				if (packZstdStreamCache.contains(filepath)) {
					res = packZstdStreamCache.at(filepath);
				}
				else {
					std::shared_ptr<Formats::IO::Stream> decompressed = Formats::Resources::ZSTD::ZSTDBackend::Decompress(stream);
					if (decompressed != nullptr) {
						res = decompressed;
						packZstdStreamCache.insert(std::make_pair(filepath, decompressed));
					}
					else
						res = nullptr; // Not sure what to do here.. filename ends in zs but either can't be decompressed or isn't compressed.
				}
			}
			else {
				res = stream;
			}
		}
		return res;
	}
	std::shared_ptr<Formats::IO::Stream> _Filesystem::OpenWriteStream(std::string filepath) {
		std::filesystem::path proximatedFilepath = std::filesystem::proximate(std::filesystem::path(filepath), "Work");
		
		std::vector<std::string> realDirs = GetRealDirs(filepath);
		std::vector<std::shared_ptr<Formats::IO::Stream>> streams;
		streams.reserve(realDirs.size());

		std::string dumpDir;
		{
			std::shared_lock<std::shared_mutex>(mDumpDirMutex);
			dumpDir = mDumpDir;
		}
		std::string writeDir;
		{
			std::shared_lock<std::shared_mutex>(mWriteDirMutex);
			writeDir = mWriteDir;
		}

		if (realDirs.size() == 0) // Support new files being created
			realDirs.push_back(writeDir);

		for (std::string realDir : realDirs) {
			if (std::filesystem::path(realDir).generic_string() == (std::filesystem::path(dumpDir) / "romfs").generic_string())
				realDir = writeDir;

			std::filesystem::path proximatedRealDir = std::filesystem::proximate(std::filesystem::path(realDir), "Work");
			PHYSFS_mkdir(proximatedRealDir.parent_path().generic_string().c_str());

			// Create the file
			PHYSFS_File* realDirFile = PHYSFS_openWrite(proximatedRealDir.generic_string().c_str());
			PHYSFS_File* romfsRealDirFile = PHYSFS_openRead(realDir.c_str());
			if (romfsRealDirFile) {
				std::shared_ptr<Formats::IO::Stream> cachedDecompressedStream = packZstdStreamCache.at(realDir);

				PHYSFS_sint64 len = cachedDecompressedStream->GetBufferLength();
				F_U8* buf = new F_U8[len];
				cachedDecompressedStream->ReadBytes(buf, len);
				cachedDecompressedStream->Seek(0);

				PHYSFS_writeBytes(realDirFile, buf, len);
				delete[] buf;
			}

			// Write dir juggling
			std::string oldWriteDir = PHYSFS_getWriteDir();

			// Set the new write dir to our archive
			PHYSFS_setWriteDir((std::filesystem::path(oldWriteDir) / proximatedRealDir).generic_string().c_str());

			// Make all the directories we need
			PHYSFS_mkdir(proximatedFilepath.parent_path().generic_string().c_str());

			// Open the file in the new write dir
			PHYSFS_File* file = PHYSFS_openWrite(proximatedFilepath.generic_string().c_str());

			// Write dir juggling
			PHYSFS_setWriteDir(oldWriteDir.c_str());

			AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::MountHandle>(realDirFile, "Work", proximatedRealDir.generic_string().c_str(), false, mFloatDirs));
			ExecutePHYSFSCallQueue();
			AddFloatDir(proximatedRealDir.generic_string());

			streams.push_back(std::make_shared<TotkToolkit::IO::Streams::Physfs::Physfs>(file, true));
		}

		return std::make_shared<TotkToolkit::IO::Streams::Multi::Multi>(streams);
	}
	std::string _Filesystem::GetRealDir(std::string path) {
		return PHYSFS_getRealDir(path.c_str());
	}
	std::vector<std::string> _Filesystem::GetRealDirs(std::string path) {
		std::vector<std::string> res;

		const char** realDirs;
		int realDirsLength;
		PHYSFS_getRealDirs(path.c_str(), &realDirs, &realDirsLength);
		res.reserve(realDirsLength);
		for (int i = 0; i < realDirsLength; i++)
			res.push_back(realDirs[i]);

		return res;
	}

	std::shared_ptr<Formats::IO::Stream> _Filesystem::OpenCacheReadStream(std::string filepath) {
		const char* prefDir = PHYSFS_getPrefDir("Nintenstudio", "TotkToolkit");
		return TotkToolkit::IO::Streams::File::File::Factory(std::make_shared<std::fstream>((std::filesystem::path(prefDir) / filepath), std::ios::in | std::ios::binary));
	}
	std::shared_ptr<Formats::IO::Stream> _Filesystem::OpenCacheWriteStream(std::string filepath) {
		const char* prefDir = PHYSFS_getPrefDir("Nintenstudio", "TotkToolkit");
		std::filesystem::create_directories((std::filesystem::path(prefDir) / filepath).parent_path());
		return TotkToolkit::IO::Streams::File::File::Factory(std::make_shared<std::fstream>((std::filesystem::path(prefDir) / filepath), std::ios::out | std::ios::binary));
	}

	std::vector<std::string> _Filesystem::EnumerateFiles(std::string path) {
		std::vector<std::string> res;

		char** files = PHYSFS_enumerateFiles(path.c_str());

		char* currentFile;
		for (F_U32 i = 0; currentFile = files[i], currentFile != nullptr; i++) {
			if (std::string(currentFile).find_first_of('.') != std::string::npos)
				res.push_back(currentFile);
			// PHYSFS_isDirectory is incredibly slow...
			//if (!PHYSFS_isDirectory((std::filesystem::path(path) / std::filesystem::path(currentFile)).generic_string().c_str()))
			//	res.push_back(currentFile);
		}

		return res;
	}
	std::vector<std::string> _Filesystem::EnumerateDirectories(std::string path) {
		std::vector<std::string> res;

		char** files = PHYSFS_enumerateFiles(path.c_str());

		char* currentFile;
		for (F_U32 i = 0; currentFile = files[i], currentFile != nullptr; i++) {
			if (std::string(currentFile).find_first_of('.') == std::string::npos)
				res.push_back(currentFile);
			// PHYSFS_isDirectory is incredibly slow...
			//if (PHYSFS_isDirectory((std::filesystem::path(path) / std::filesystem::path(currentFile)).generic_string().c_str()))
			//	res.push_back(currentFile);
		}

		return res;
	}
	std::vector<std::string> _Filesystem::SearchFilenamesByRegex(std::string dir, std::string regex, std::shared_ptr<std::atomic<bool>> continueCondition) {
		SearchFilenamesByRegexCallbackData callbackRes(regex, continueCondition);

		PHYSFS_enumerate(dir.c_str(), searchFilenamesByRegexCallback, &callbackRes);

		return callbackRes.mRetPaths;
	}
	std::vector<std::string> _Filesystem::SearchFilenamesByExtension(std::string dir, std::string extension, std::shared_ptr<std::atomic<bool>> continueCondition) {
		SearchFilenamesByExtensionCallbackData callbackRes(extension, continueCondition);

		PHYSFS_enumerate(dir.c_str(), searchFilenamesByExtensionCallback, &callbackRes);

		return callbackRes.mRetPaths;
	}

	std::shared_ptr<TotkToolkit::Threading::TaskReport> _Filesystem::GetLoadTaskReport() {
		std::shared_ptr<TotkToolkit::Threading::TaskReport> res = std::make_shared<TotkToolkit::Threading::TaskReport>(TotkToolkit::UI::Localization::TranslationSource::GetText("PREPARE_FILES"), 0);

		std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> mountArchivesTasks = GetTasks(TotkToolkit::Threading::TaskType::IO_FILESYSTEM_MOUNTARCHIVES);
		if (mountArchivesTasks.size() == 0)
			return nullptr;

		for (std::shared_ptr<TotkToolkit::Threading::Task> task : mountArchivesTasks)
			res->AddSubTaskReport(task->GetTaskReport());
		return res;
	}

	void _Filesystem::HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) {
		switch (notice->mType) {
		case TotkToolkit::Messaging::NoticeType::CONFIGURATION_SETTINGS_CHANGE_DUMPDIR: {
			std::shared_ptr<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::DumpDir> castNotice = std::static_pointer_cast<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::DumpDir>(notice);

			InitThread();
			SyncThread();
			Unmount(castNotice->mOldDumpDir);
			SetDumpDir(castNotice->mNewDumpDir);
			Mount((std::filesystem::path(mDumpDir) / std::filesystem::path("romfs")).generic_string(), "Work");

			// Initialize ZSTD dictionaries
			std::shared_ptr<Formats::IO::Stream> ZsDicPack = TotkToolkit::IO::Filesystem.OpenReadStream("Work/Pack/ZsDic.pack.zs");
			if (ZsDicPack != nullptr) {
				MountStream(ZsDicPack, "ZsDic.pack.zs", "Work");

				std::shared_ptr<Formats::IO::Stream> ZsZsdic = TotkToolkit::IO::Filesystem.OpenReadStream("Work/zs.zsdic");
				if (ZsZsdic != nullptr)
					Formats::Resources::ZSTD::ZSTDBackend::AddDict(ZsZsdic);
				std::shared_ptr<Formats::IO::Stream> BcettBymlZsdic = TotkToolkit::IO::Filesystem.OpenReadStream("Work/bcett.byml.zsdic");
				if (BcettBymlZsdic != nullptr)
					Formats::Resources::ZSTD::ZSTDBackend::AddDict(BcettBymlZsdic);
				std::shared_ptr<Formats::IO::Stream> PackZsDic = TotkToolkit::IO::Filesystem.OpenReadStream("Work/pack.zsdic");
				if (PackZsDic != nullptr)
					Formats::Resources::ZSTD::ZSTDBackend::AddDict(PackZsDic);

				// Sarc archiver zstd
				zstd_io_add_dict("Work/zs.zsdic");
				zstd_io_add_dict("Work/bcett.byml.zsdic");
				zstd_io_add_dict("Work/pack.zsdic");
			}


			// Mount archives
			std::shared_ptr<TotkToolkit::Threading::Task> mountArchivesTask = std::make_shared<TotkToolkit::Threading::Tasks::IO::Filesystem::MountArchives>((std::filesystem::path(mDumpDir) / "romfs" / "Pack").generic_string(), [this]() -> void { Float(); DeinitThread(); /*sMountArchivesTask.store(nullptr);*/ });
			AddTask(mountArchivesTask);
			mountArchivesTask->ExecuteAsync();

			TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::IO::Filesystem::Mount::Romfs>());

			return;
		}
		case TotkToolkit::Messaging::NoticeType::CONFIGURATION_SETTINGS_CHANGE_WRITEDIR: {
			std::shared_ptr<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::WriteDir> castNotice = std::static_pointer_cast<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::WriteDir>(notice);

			InitThread();
			SyncThread();
			SetWriteDir(castNotice->mNewWriteDir);
			TotkToolkit::IO::Filesystem.Unmount(castNotice->mOldWriteDir);
			TotkToolkit::IO::Filesystem.Mount(mWriteDir, "Work");

			// TOTKTOOLKIT_TODO_FUNCTIONAL: Figure out what to do about dictionaries; they should be mounted before doing this.

			// Mount archives
			std::shared_ptr<TotkToolkit::Threading::Task> mountArchivesTask = std::make_shared<TotkToolkit::Threading::Tasks::IO::Filesystem::MountArchives>((std::filesystem::path(mWriteDir) / "Pack").generic_string(), [this]() -> void { Float(); DeinitThread(); /*sMountArchivesTask.store(nullptr);*/ },
				[this](std::string dir) -> void {
					AddFloatDir(dir); // Float all the write directories above non-write directories
				}
			);
			AddTask(mountArchivesTask);
			mountArchivesTask->ExecuteAsync();

			TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::IO::Filesystem::Mount::WriteDir>());
			return;
		}
		default:
			return;
		}
	}

	void _Filesystem::AddFloatDir(std::string dir) {
		std::unique_lock<std::shared_mutex> lock;
		mFloatDirs.push_back(dir);
	}

	void _Filesystem::RemoveFloatDir(std::string dir) {
		std::unique_lock<std::shared_mutex> lock;
		mFloatDirs.erase(std::remove(mFloatDirs.begin(), mFloatDirs.end(), dir), mFloatDirs.end());
	}
}