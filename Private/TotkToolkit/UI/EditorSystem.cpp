#include <TotkToolkit/UI/EditorSystem.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>

namespace TotkToolkit::UI {
	void EditorSystem::Init() {
		TotkToolkit::Messaging::NoticeBoard::AddReceiver(&sExternalReceiver);
	}

	void EditorSystem::Draw() {
		sEditors = sNextEditors;

		for (F_S32 i = sEditors.size() - 1; i >= 0; i--) {
			if (!sEditors[i]->IsOpen())
				sEditors.erase(sEditors.begin() + i);
		}

		for (std::shared_ptr<TotkToolkit::UI::Items::Windows::Editor> editor : sEditors)
			editor->Draw();
	}

	void EditorSystem::AddEditor(std::shared_ptr<TotkToolkit::UI::Items::Windows::Editor> editor) {
		sNextEditors.push_back(editor);
	}



	std::vector<std::shared_ptr<TotkToolkit::UI::Items::Windows::Editor>> EditorSystem::sEditors;
	std::vector<std::shared_ptr<TotkToolkit::UI::Items::Windows::Editor>> EditorSystem::sNextEditors;

	TotkToolkit::Messaging::ExternalReceivers::UI::EditorSystem EditorSystem::sExternalReceiver;
}