#include <TotkToolkit/UI/Items/Windows/Editors/Texts/Plain.h>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
	Plain::Plain(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::Text(fileHandle, name, open), TotkToolkit::Resource(fileHandle) {

	}

	bool Plain::Parse() {

		return true;
	}

	bool Plain::Serialize() {

		return true;
	}
}