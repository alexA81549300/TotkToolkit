#include <TotkToolkit/UI/Items/Windows/Editors/BGYML.h>

namespace TotkToolkit::UI::Items::Windows::Editors {
	BGYML::BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(fileHandle, name, open) {

	}
	BGYML::BGYML(std::shared_ptr<TotkToolkit::Resources::BGYML> resource, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(resource, name, open) {

	}
}