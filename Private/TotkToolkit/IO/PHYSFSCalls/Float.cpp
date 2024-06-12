#include <TotkToolkit/IO/PHYSFSCalls/Float.h>

#include <physfs.h>

namespace TotkToolkit::IO::PHYSFSCalls {
	void Float::Execute() {
		for (std::string dir : mFloatDirs) {
			PHYSFS_moveInSearchPath(dir.c_str(), 0);
		}
	}
}