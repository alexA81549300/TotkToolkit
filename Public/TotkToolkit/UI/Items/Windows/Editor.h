#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/Resource.h>

#include <Formats/Resource.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows {
	class Editor : virtual public TotkToolkit::Resource, public TotkToolkit::UI::Items::Window {
	public:
		Editor(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);
		Editor(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open);

		virtual void Draw() override;
		virtual void DrawContents() override;

	protected:
		bool mModified = false;
	};
}