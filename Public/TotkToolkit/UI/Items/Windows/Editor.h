#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/Resource.h>

#include <Formats/Resource.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows {
	class Editor : public TotkToolkit::UI::Items::Window {
	public:
		Editor(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);
		Editor(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open);

		virtual bool Parse() = 0;
		virtual bool Serialize() = 0;
		virtual bool RequestParse(Formats::ResourceParsedCallback callback) { return false; } // TOTKTOOLKIT_TODO_FUNCTIONAL Implement ocne implemented in Formats

		virtual void Draw() override;
		virtual void DrawContents() override;

	protected:
		bool mModified = false;
	};
}