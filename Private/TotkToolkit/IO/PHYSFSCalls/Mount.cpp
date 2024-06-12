#include <TotkToolkit/IO/PHYSFSCalls/Mount.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <physfs.h>

namespace TotkToolkit::IO::PHYSFSCalls {
	void Mount::Execute() {
		int mountSuccess = PHYSFS_mount(mPath.c_str(), mMountPoint.c_str(), mAppendToPath);
		for (std::string dir : mFloatDirs) {
			PHYSFS_moveInSearchPath(dir.c_str(), 0);
		}
		if (mountSuccess && mNotifyFilesChanged && !mNotifiedFilesChanged) {
			TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>());
			mNotifiedFilesChanged = true;
		}
	}
}