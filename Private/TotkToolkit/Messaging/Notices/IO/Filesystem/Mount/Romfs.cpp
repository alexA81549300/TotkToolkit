#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/Romfs.h>

namespace TotkToolkit::Messaging::Notices::IO::Filesystem::Mount {
	Romfs::Romfs() :
		TotkToolkit::Messaging::Notice::Notice(TotkToolkit::Messaging::NoticeType::IO_FILESYSTEM_MOUNT_ROMFS)
	{
	}
}