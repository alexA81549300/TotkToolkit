#include <TotkToolkit/UI/Items/Windows/Editors/Texts/BYML.h>

#include <imgui.h>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
	BYML::BYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::Text(fileHandle, name, open), mBYML(std::make_shared<TotkToolkit::Resources::BYML>(fileHandle)) {
		mTextEditor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::Yaml());
	}

	bool BYML::Parse() {
		if (!mBYML->Parse())
			return false;

		mSavedText = mBYML->ToYAML();
		mTextEditor.SetText(mSavedText);

		return true;
	}

	bool BYML::Serialize() {
		mBYML->LoadYAML(mTextEditor.GetText());
		return mBYML->Serialize();
	}
}