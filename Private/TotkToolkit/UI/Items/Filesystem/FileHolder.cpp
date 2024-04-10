#include <TotkToolkit/UI/Items/Filesystem/FileHolder.h>

#include "imgui.h"

namespace TotkToolkit::UI::Items::Filesystem {
	FileHolder::FileHolder() : mFile() {

	}
	FileHolder::FileHolder(TotkToolkit::UI::Items::Filesystem::File file) : mFile(file) {

	}

	void FileHolder::Draw() {
		ImVec2 filePos = ImGui::GetCursorScreenPos();
		mFile.Draw();
		ImVec2 fileEndPos = ImVec2(filePos.x + ImGui::GetItemRectSize().x, filePos.y + ImGui::GetItemRectSize().y);
		
		ImGui::GetWindowDrawList()->AddRect(filePos, fileEndPos, IM_COL32(ImGui::GetStyle().Colors[ImGuiCol_Border].x * 255, ImGui::GetStyle().Colors[ImGuiCol_Border].y * 255, ImGui::GetStyle().Colors[ImGuiCol_Border].z * 255, ImGui::GetStyle().Colors[ImGuiCol_Border].w * 255));

		if (ImGui::BeginDragDropTarget()) {
			ImGuiDragDropFlags target_flags = 0;
			//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
			//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TOTKTOOLKIT_File", target_flags))
			{
				mFile = *(TotkToolkit::UI::Items::Filesystem::File*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
	}

	TotkToolkit::UI::Items::Filesystem::File FileHolder::GetFile() const {
		return mFile;
	}
}