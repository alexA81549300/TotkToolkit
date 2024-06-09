#include <TotkToolkit/IO/Filesystem.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/IO/PHYSFSCalls/Mount.h>
#include <TotkToolkit/IO/PHYSFSCalls/MountMemory.h>
#include <TotkToolkit/IO/PHYSFSCalls/Unmount.h>
#include <TotkToolkit/IO/PHYSFSCalls/SetWriteDir.h>
#include <TotkToolkit/IO/PHYSFSCall.h>
#include <TotkToolkit/IO/Streams/Physfs/Physfs.h>
#include <TotkToolkit/IO/Streams/Multi/Multi.h>
#include <Formats/Resources/ZSTD/ZSTDBackend.h>
#include <archiver_sarc.h>
#include <physfs.h>
#include <filesystem>
#include <map>
#include <vector>
#include <regex>
#include <iostream>
#include <shared_mutex>
#include <thread>

std::map<std::thread::id, std::vector<std::shared_ptr<TotkToolkit::IO::PHYSFSCall>>> sPHYSFSCallQueue;
std::shared_mutex sPHYSFSCallQueueMutex;
std::vector<std::shared_ptr<TotkToolkit::IO::PHYSFSCall>> sEntirePHYSFSCallQueue; // Used to get new threads up-to-date
std::shared_mutex sEntirePHYSFSCallQueueMutex;
void AddPHYSFSCall(std::shared_ptr<TotkToolkit::IO::PHYSFSCall> call) {
	{
		std::unique_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
		for (auto& it : sPHYSFSCallQueue) {
			it.second.push_back(call);
		}
	}
	{
		std::unique_lock<std::shared_mutex> lock(sEntirePHYSFSCallQueueMutex);
		sEntirePHYSFSCallQueue.push_back(call);
	}
}
void ExecutePHYSFSCallQueue() {
	std::shared_lock<std::shared_mutex> lock(sPHYSFSCallQueueMutex);
	for (std::shared_ptr<TotkToolkit::IO::PHYSFSCall> call : sPHYSFSCallQueue.at(std::this_thread::get_id())) {
		call->Execute();
	}

	sPHYSFSCallQueue.at(std::this_thread::get_id()).clear();
}

void BindCurrentThreadContext() {
	static std::map<std::thread::id, PHYSFS_Context> lsThreadContexts;
	static std::shared_mutex lsThreadContextsMutex;
	
	{
		std::shared_lock<std::shared_mutex> lock(lsThreadContextsMutex);
		if (lsThreadContexts.contains(std::this_thread::get_id())) {
			PHYSFS_bindContext(lsThreadContexts.at(std::this_thread::get_id()));
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
		std::unique_lock<std::shared_mutex> lock(lsThreadContextsMutex);
		lsThreadContexts.insert({std::this_thread::get_id(), context});
	}
	{
		std::unique_lock<std::shared_mutex> queueLock(sPHYSFSCallQueueMutex);
		std::shared_lock<std::shared_mutex> entireQueueLock(sEntirePHYSFSCallQueueMutex);
		sPHYSFSCallQueue.insert({std::this_thread::get_id(), sEntirePHYSFSCallQueue});
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

	std::string filePath = (std::filesystem::path(origdir) / std::filesystem::path(fname)).generic_string().c_str();

	if (std::string(fname).ends_with(callbackData->mExtension))
		callbackData->mRetPaths.push_back(filePath);

	if (filePath.find_first_of('.') == std::string::npos) {
	// PHYSFS_isDirectory is incredibly slow...
	//if (PHYSFS_isDirectory(filePath.c_str())) {
		PHYSFS_enumerate(filePath.c_str(), searchFilenamesByExtensionCallback, callbackData);

		if (!*callbackData->mContinueCondition)
			return PHYSFS_ENUM_STOP;
	}

	return PHYSFS_EnumerateCallbackResult::PHYSFS_ENUM_OK;
}


class MountHandle : public TotkToolkit::IO::PHYSFSCall {
public:
	MountHandle(PHYSFS_File* handle, std::string mountPoint, std::string newDir, bool appendToPath, bool notifyFileChange = true) : mHandle(handle), mMountPoint(mountPoint), mNewDir(newDir), mAppendToPath(appendToPath), mNotifyFileChange(notifyFileChange) {

	}

	virtual void Execute() override {
		if (PHYSFS_mountHandle(mHandle, mNewDir.c_str(), mMountPoint.c_str(), mAppendToPath) && mNotifyFileChange)
			TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>());
	}

protected:
	PHYSFS_File* mHandle;
	std::string mMountPoint;
	std::string mNewDir;
	bool mAppendToPath;
	bool mNotifyFileChange;
};

namespace TotkToolkit::IO {
	void Filesystem::Init() {
		PHYSFS_init("");
		TotkToolkit::Messaging::NoticeBoard::AddReceiver(&sExternalReceiver);
	}

	void Filesystem::InitThread() {
		BindCurrentThreadContext();
	}

	void Filesystem::SyncThread() {
		ExecutePHYSFSCallQueue();
	}

	void Filesystem::Mount(std::string path, std::string mountPoint, bool notifyFileChange) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::Mount>(path, mountPoint, false, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void Filesystem::MountStream(std::shared_ptr<Formats::IO::Stream> stream, std::string filename, std::string mountPoint, bool notifyFileChange) {
		std::shared_ptr<F_U8[]> buffer = stream->GetBuffer();
		F_UT bufferLength = stream->GetBufferLength();
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::MountMemory>(buffer, bufferLength, nullptr, filename, mountPoint, true, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void Filesystem::Unmount(std::string path, bool notifyFileChange) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::Unmount>(path, notifyFileChange));
		ExecutePHYSFSCallQueue();
	}
	void Filesystem::SetDumpDir(std::string dir) {
		mDumpDir = dir;
	}
	void Filesystem::SetWriteDir(std::string dir) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::SetWriteDir>(dir));
		ExecutePHYSFSCallQueue();
		mWriteDir = dir;
	}

	// TOTKTOOLKIT_TODO_FUNCTIONAL: Implement ZSTD on a physfs level to save hella memory
	std::map<std::string, std::shared_ptr<Formats::IO::Stream>> packZstdStreamCache;
	std::shared_ptr<Formats::IO::Stream> Filesystem::OpenReadStream(std::string filepath) {
		std::shared_ptr<Formats::IO::Stream> res;
		
		// Temporarily mount the write dir for reading.
		// This is done here so its always at the end of the search path.
		// Using raw PHYSFS call because this is undone on return.
		if (PHYSFS_getWriteDir() != nullptr)
			PHYSFS_mount(PHYSFS_getWriteDir(), "", false);
		
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

		// The write dir's mount was temporary, remember?
		if (PHYSFS_getWriteDir() != nullptr)
			PHYSFS_unmount(PHYSFS_getWriteDir());
		return res;
	}
	std::shared_ptr<Formats::IO::Stream> Filesystem::OpenWriteStream(std::string filepath) {
		std::filesystem::path proximatedFilepath = std::filesystem::proximate(std::filesystem::path(filepath), "Work");
		
		std::vector<std::string> realDirs = GetRealDirs(filepath);
		std::vector<std::shared_ptr<Formats::IO::Stream>> streams;
		streams.reserve(realDirs.size());

		if (realDirs.size() == 0) // Support new files being created
			realDirs.push_back(mWriteDir);

		for (std::string realDir : realDirs) {
			if (std::filesystem::path(realDir).generic_string() == (std::filesystem::path(mDumpDir) / "romfs").generic_string())
				realDir = mWriteDir;

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
			//PHYSFS_close(realDirFile);

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

			//AddPHYSFSCall(std::make_shared<MountHandle>(realDirFile, "Work", proximatedRealDir.generic_string().c_str(), 0));
			//ExecutePHYSFSCallQueue();

			streams.push_back(std::make_shared<TotkToolkit::IO::Streams::Physfs::Physfs>(file, true));
		}

		return std::make_shared<TotkToolkit::IO::Streams::Multi::Multi>(streams);
	}
	std::string Filesystem::GetRealDir(std::string path) {
		return PHYSFS_getRealDir(path.c_str());
	}
	std::vector<std::string> Filesystem::GetRealDirs(std::string path) {
		std::vector<std::string> res;

		const char** realDirs;
		int realDirsLength;
		PHYSFS_getRealDirs(path.c_str(), &realDirs, &realDirsLength);
		res.reserve(realDirsLength);
		for (int i = 0; i < realDirsLength; i++)
			res.push_back(realDirs[i]);

		return res;
	}

	std::vector<std::string> Filesystem::EnumerateFiles(std::string path) {
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
	std::vector<std::string> Filesystem::EnumerateDirectories(std::string path) {
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
	std::vector<std::string> Filesystem::SearchFilenamesByRegex(std::string dir, std::string regex, std::shared_ptr<std::atomic<bool>> continueCondition) {
		SearchFilenamesByRegexCallbackData callbackRes(regex, continueCondition);

		PHYSFS_enumerate(dir.c_str(), searchFilenamesByRegexCallback, &callbackRes);

		return callbackRes.mRetPaths;
	}
	std::vector<std::string> Filesystem::SearchFilenamesByExtension(std::string dir, std::string extension, std::shared_ptr<std::atomic<bool>> continueCondition) {
		SearchFilenamesByExtensionCallbackData callbackRes(extension, continueCondition);
		
		PHYSFS_enumerate(dir.c_str(), searchFilenamesByExtensionCallback, &callbackRes);

		return callbackRes.mRetPaths;
	}

	TotkToolkit::Messaging::ExternalReceivers::IO::Filesystem Filesystem::sExternalReceiver;
	std::string Filesystem::mDumpDir;
	std::string Filesystem::mWriteDir;
}