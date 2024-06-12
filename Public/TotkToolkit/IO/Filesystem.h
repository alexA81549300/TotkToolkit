#pragma once

#include <TotkToolkit/Messaging/Receiver.h>
#include <TotkToolkit/System.h>
#include <Formats/IO/Stream.h>
#include <memory>
#include <vector>
#include <string>
#include <atomic>

namespace TotkToolkit::IO {
	class _Filesystem : public TotkToolkit::System, public TotkToolkit::Messaging::Receiver {
	public:
		void Init();

		/// @brief Should be called at least once on new threads that use filesystem.
		void InitThread();
		void DeinitThread();
		void SyncThread();

		void Mount(std::string path, std::string mountPoint, bool notifyFileChange = true, bool deferredFloating = false);
		void MountStream(std::shared_ptr<Formats::IO::Stream> stream, std::string filename, std::string mountPoint, bool notifyFileChange = true, bool deferredFloating = false);
		void Unmount(std::string path, bool notifyFileChange = true);
		void Float();
		bool TempMount(std::string path, std::string mountPoint);
		bool TempUnmount(std::string path);
		std::string GetMountPoint(std::string path);
		std::string GetDumpDir();
		void SetDumpDir(std::string dir);
		std::string GetWriteDir();
		void SetWriteDir(std::string dir);

		std::shared_ptr<Formats::IO::Stream> OpenReadStream(std::string filepath);
		std::shared_ptr<Formats::IO::Stream> OpenWriteStream(std::string filepath);
		std::string GetRealDir(std::string path);
		std::vector<std::string> GetRealDirs(std::string path);

		std::vector<std::string> EnumerateFiles(std::string path);
		std::vector<std::string> EnumerateDirectories(std::string path);
		std::vector<std::string> SearchFilenamesByRegex(std::string dir, std::string regex, std::shared_ptr<std::atomic<bool>> continueCondition = std::make_shared<std::atomic<bool>>(true));
		std::vector<std::string> SearchFilenamesByExtension(std::string dir, std::string extension, std::shared_ptr<std::atomic<bool>> continueCondition = std::make_shared<std::atomic<bool>>(true));

		std::shared_ptr<TotkToolkit::Threading::TaskReport> GetLoadTaskReport();

		virtual void HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) override;

	protected:
		void AddFloatDir(std::string dir);
		void RemoveFloatDir(std::string dir);

		std::string mDumpDir; std::shared_mutex mDumpDirMutex;
		std::string mWriteDir; std::shared_mutex mWriteDirMutex;
		std::vector<std::string> mFloatDirs; std::shared_mutex mFloatDirsMutex;
	};

	extern _Filesystem Filesystem;
}