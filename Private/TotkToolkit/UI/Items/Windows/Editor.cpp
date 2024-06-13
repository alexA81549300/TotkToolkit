#include <TotkToolkit/UI/Items/Windows/Editor.h>

#include <imgui.h>

namespace TotkToolkit::UI::Items::Windows {
	Editor::Editor(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Window(name, open) {
		mWindowFlags |= ImGuiWindowFlags_MenuBar;
	}
	Editor::Editor(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open) : TotkToolkit::UI::Items::Window(name, open) {
		mWindowFlags |= ImGuiWindowFlags_MenuBar;
	}

	void Editor::Draw() {
		bool unCollapsed;
		if (mModified)
			unCollapsed = ImGui::Begin(AppendExclusiveIdentifier(mName + "*").c_str(), mOpen, mWindowFlags);
		else
			unCollapsed = ImGui::Begin(AppendExclusiveIdentifier(mName).c_str(), mOpen, mWindowFlags);
		if (unCollapsed) {
			DrawContents();
		}
		ImGui::End();
	}

	void Editor::DrawContents() {
		ImGui::PushID(GetIdentifier().c_str());
		ImGui::BeginMenuBar();
		if (ImGui::MenuItem("Save"))
			Serialize();
		ImGui::EndMenuBar();
		ImGui::PopID();
	}
}