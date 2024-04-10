#include <TotkToolkit/UI/Items/Windows/Editors/BGYML.h>

namespace TotkToolkit::UI::Items::Windows::Editors {
	BGYML::BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(fileHandle, name, open), TotkToolkit::Resources::BYML(fileHandle), TotkToolkit::Resource(fileHandle) {

	}
	BGYML::BGYML(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(resource, name, open), TotkToolkit::Resources::BYML(resource), TotkToolkit::Resource(resource) {

	}
}