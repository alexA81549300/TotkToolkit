#include <TotkToolkit/Resources/PlainText.h>

#include <imgui.h>

namespace TotkToolkit::Resources {
	PlainText::PlainText(TotkToolkit::IO::FileHandle fileHandle) : TotkToolkit::Resource(fileHandle) {}
	PlainText::PlainText(std::shared_ptr<Formats::Resource> resource) : TotkToolkit::Resource(resource) {}

	bool PlainText::Parse() {
		return true;
	}

	bool PlainText::Serialize() {
		return true;
	}
}