#pragma once

#include <TotkToolkit/UI/Items/Windows/Editor.h>
#include <TotkToolkit/Resources/BGYML.h>

#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors {
	class BGYML : public TotkToolkit::UI::Items::Windows::Editor {
	public:
		BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);
		BGYML(std::shared_ptr<TotkToolkit::Resources::BGYML> resource, std::string name, bool* open);
	};
}