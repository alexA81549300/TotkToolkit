#include <TotkToolkit/IO/Filesystem.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/IO/PHYSFSCalls/Mount.h>
#include <TotkToolkit/IO/PHYSFSCalls/MountMemory.h>
#include <TotkToolkit/IO/PHYSFSCalls/Unmount.h>
#include <TotkToolkit/IO/PHYSFSCalls/SetWriteDir.h>
#include <TotkToolkit/IO/PHYSFSCall.h>
#include <TotkToolkit/IO/Streams/Physfs/Physfs.h>
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
	void Filesystem::SetWriteDir(std::string dir) {
		AddPHYSFSCall(std::make_shared<TotkToolkit::IO::PHYSFSCalls::SetWriteDir>(dir));
		ExecutePHYSFSCallQueue();
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
			std::shared_ptr<TotkToolkit::IO::Streams::Physfs::Physfs> stream = std::make_shared<TotkToolkit::IO::Streams::Physfs::Physfs>(file);

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
		std::filesystem::path realDir = std::filesystem::proximate(std::filesystem::path(GetRealDir(filepath)), "Work");
		PHYSFS_mkdir(realDir.parent_path().generic_string().c_str());
		
		const char** test;
		int test2;
		PHYSFS_getRealDirs(filepath.c_str(), &test, &test2);

		// Create the file
		PHYSFS_File* realDirFile = PHYSFS_openWrite("test.sarc");
		PHYSFS_close(realDirFile);

		// Test 1
		std::string oldWriteDir = PHYSFS_getWriteDir();

		// Test 2
		PHYSFS_setWriteDir((std::filesystem::path(oldWriteDir) / "test.sarc").generic_string().c_str());

		// Do whatever this is
		PHYSFS_mkdir(std::filesystem::path(filepath).parent_path().generic_string().c_str());

		// Open the file in the new context
		PHYSFS_File* file = PHYSFS_openWrite(filepath.c_str());

		//PHYSFS_setWriteDir(oldWriteDir.c_str());

		return std::make_shared<TotkToolkit::IO::Streams::Physfs::Physfs>(file);
	}
	std::string Filesystem::GetRealDir(std::string path) {
		return PHYSFS_getRealDir(path.c_str());
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
}