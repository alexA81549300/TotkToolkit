#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/WriteDir.h>

namespace TotkToolkit::Messaging::Notices::IO::Filesystem::Mount {
	WriteDir::WriteDir() :
		TotkToolkit::Messaging::Notice::Notice(TotkToolkit::Messaging::NoticeType::IO_FILESYSTEM_MOUNT_WRITEDIR)
	{
	}
}