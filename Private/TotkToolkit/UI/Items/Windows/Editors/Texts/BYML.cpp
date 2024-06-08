#include <TotkToolkit/UI/Items/Windows/Editors/Texts/BYML.h>

#include <imgui.h>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
	BYML::BYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::Text(fileHandle, name, open), TotkToolkit::Resources::BYML(fileHandle), TotkToolkit::Resource(fileHandle) {
		mTextEditor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::Yaml());
	}

	bool BYML::Parse() {
		mBYML = Formats::Resources::BYML::BYML::Factory(mFileHandle.OpenReadStream());
		if (mBYML == nullptr)
			return false;

		mSavedText = mBYML->ToYAML();
		mTextEditor.SetText(mSavedText);

		return true;
	}

	bool BYML::Serialize() {
		mBYML->LoadYAML(mTextEditor.GetText());

		std::shared_ptr<Formats::IO::Stream> oldStream = mBYML->GetStream();
		mBYML->SetStream(mFileHandle.OpenWriteStream());
		bool success = mBYML->Serialize();
		mBYML->SetStream(oldStream);
		return success;
	}
}