#include <TotkToolkit/UI/Items/Windows/Editors/Texts/PlainText.h>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
	PlainText::PlainText(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::Text(fileHandle, name, open), mPlainText(std::make_shared<TotkToolkit::Resources::PlainText>(fileHandle)) {

	}

	bool PlainText::Parse() {

		return true;
	}

	bool PlainText::Serialize() {

		return true;
	}
}