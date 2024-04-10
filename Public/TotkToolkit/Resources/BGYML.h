#pragma once

#include <TotkToolkit/Resource.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <Formats/Resources/BYML/BYML.h>
#include <string>
#include <memory>

namespace TotkToolkit::Resources {
	class BGYML : virtual public TotkToolkit::Resource {
	public:
		BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

		virtual bool Parse() override;
		virtual bool Parse_() override;
		virtual bool Serialize() override;

	protected:
		TotkToolkit::IO::FileHandle mFileHandle;
		std::shared_ptr<Formats::Resources::BYML::BYML> mBYML;
		bool mModified;
	};
}