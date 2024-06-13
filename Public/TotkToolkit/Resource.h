#pragma once

#include <TotkToolkit/IO/FileHandle.h>
#include <Formats/Resource.h>
#include <memory>

namespace TotkToolkit {
	class Resource : public Formats::Resource {
	public:
		Resource(TotkToolkit::IO::FileHandle fileHandle);
		Resource(std::shared_ptr<Formats::Resource> resource);

		virtual bool Parse() override = 0;
		virtual bool Serialize() override = 0;
		virtual bool RequestParse(Formats::ResourceParsedCallback callback) override { return false; } // TOTKTOOLKIT_TODO_FUNCTIONAL Implement ocne implemented in Formats

	protected:
		TotkToolkit::IO::FileHandle mFileHandle;
	};
}