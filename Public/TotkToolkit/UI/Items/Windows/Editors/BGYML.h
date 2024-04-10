#pragma once

#include <TotkToolkit/UI/Items/Windows/Editor.h>
#include <TotkToolkit/Resources/BYML.h>

#include <Formats/Resources/BYML/BYML.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors {
	class BGYML : public TotkToolkit::UI::Items::Windows::Editor, virtual public TotkToolkit::Resources::BYML {
	public:
		BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);
		BGYML(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open);
	};
}