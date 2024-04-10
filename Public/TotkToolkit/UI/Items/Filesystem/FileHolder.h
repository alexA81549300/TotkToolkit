#pragma once

#include <TotkToolkit/UI/Item.h>

#include <TotkToolkit/UI/Items/Filesystem/File.h>
#include <memory>

namespace TotkToolkit::UI::Items::Filesystem {
	class FileHolder : public TotkToolkit::UI::Item {
	public:
		FileHolder();
		FileHolder(TotkToolkit::UI::Items::Filesystem::File file);

		virtual void Draw() override;

		TotkToolkit::UI::Items::Filesystem::File GetFile() const;

	protected:
		TotkToolkit::UI::Items::Filesystem::File mFile;
	};
}