#include <TotkToolkit/Resource.h>

namespace TotkToolkit {
	Resource::Resource(TotkToolkit::IO::FileHandle fileHandle) : mFileHandle(fileHandle) {

	}
	Resource::Resource(std::shared_ptr<Formats::Resource> resource) : mFileHandle() {
		// It's not currently valuable to store a base-type resource here.
		// Right now this constructor does nothing but open up a new mode of operation.
	}
}