#pragma once

#include <TotkToolkit/IO/PHYSFSCall.h>

#include <string>
#include <vector>

namespace TotkToolkit::IO::PHYSFSCalls {
	class MountHandle : public TotkToolkit::IO::PHYSFSCall {
	public:
		MountHandle(void* handle, std::string mountPoint, std::string newDir, bool appendToPath, std::vector<std::string> floatDirs, bool notifyFilesChanged = true) : mHandle(handle), mMountPoint(mountPoint), mNewDir(newDir), mAppendToPath(appendToPath), mFloatDirs(floatDirs), mNotifyFilesChanged(notifyFilesChanged) {

		}

		virtual void Execute() override;

	protected:
		void* mHandle;
		std::string mMountPoint;
		std::string mNewDir;
		bool mAppendToPath;
		std::vector<std::string> mFloatDirs;
		bool mNotifyFilesChanged;
		bool mNotifiedFilesChanged = false;
	};
}