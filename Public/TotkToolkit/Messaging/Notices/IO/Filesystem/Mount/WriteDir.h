#pragma once

#include <TotkToolkit/Messaging/Notice.h>

namespace TotkToolkit::Messaging::Notices::IO::Filesystem::Mount {
	class WriteDir : public TotkToolkit::Messaging::Notice {
	public:
		WriteDir();
	};
}