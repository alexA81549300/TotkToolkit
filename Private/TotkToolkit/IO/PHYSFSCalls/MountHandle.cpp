#pragma once

#include <TotkToolkit/IO/PHYSFSCalls/MountHandle.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <physfs.h>
#include <memory>

namespace TotkToolkit::IO::PHYSFSCalls {
	void MountHandle::Execute() {
		int mountSuccess = PHYSFS_mountHandle((PHYSFS_File*)mHandle, mNewDir.c_str(), mMountPoint.c_str(), mAppendToPath);
		for (std::string dir : mFloatDirs) {
			PHYSFS_moveInSearchPath(dir.c_str(), 0);
		}
		if (mountSuccess && mNotifyFilesChanged && !mNotifiedFilesChanged) {
			TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>());
			mNotifiedFilesChanged = true;
		}
	}
}