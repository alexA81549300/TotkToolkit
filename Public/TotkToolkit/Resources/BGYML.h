#pragma once

#include <TotkToolkit/Resource.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <Formats/Resources/BYML/BYML.h>
#include <string>
#include <memory>

namespace TotkToolkit::Resources {
	class BGYML : public TotkToolkit::Resource {
	public:
		BGYML(TotkToolkit::IO::FileHandle fileHandle);

		virtual bool Parse() override;
		virtual bool Serialize() override;

		std::string GetParentPath();
		void SetParentPath(std::string parentPath);

	protected:
		// This isn't inherited because we need to be able to construct the right version.
		std::shared_ptr<Formats::Resources::BYML::BYML> mBYML;

		std::shared_ptr<BGYML> mParent;
		bool mModified;
	};
}