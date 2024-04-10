#include <TotkToolkit/UI/Items/Filesystem/File.h>

#include <TotkToolkit/UI/EditorSystem.h>
#include <TotkToolkit/UI/Items/Windows/Editors/Texts/BYML.h>
#include <TotkToolkit/UI/Items/Windows/Editors/TXTG.h>
#include <TotkToolkit/UI/Items/Windows/Editors/BGYMLs/game/component/LifeParam.h>
#include <TotkToolkit/Resources/TexToGo.h>
#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/DumpDir.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/Messaging/Notices/Errors/UI/Items/Windows/Editor/ParseFailed.h>
#include <TotkToolkit/UI/ImGuiUtil.h>
#include <TotkToolkit/UI/Icons.h>
#include <TotkToolkit/UI/Fonts.h>
#include <imgui.h>
#include <filesystem>

namespace TotkToolkit::UI::Items::Filesystem {
    File::File() : mPath(""), TotkToolkit::UI::Item() {

    }
	File::File(std::string path) : mPath(path), TotkToolkit::UI::Item() {

	}

	void File::Draw() {
        ImGui::BeginGroup();

        std::string filename = std::filesystem::path(mPath).filename().string();

        // Get the position we're at
        ImVec2 itemPos = ImGui::GetCursorPos();
        ImVec2 itemScreenPos = ImGui::GetCursorScreenPos();

        // Get the width (& height) of the item
        float itemWidth = 4 * ImGui::GetFontSize();

        // Make the actual button the user can click on
        if (ImGui::InvisibleButton(("File " + mPath).c_str(), ImVec2(itemWidth, itemWidth))) {
            if (mPath.ends_with(".game__component__LifeParam.bgyml") || mPath.ends_with(".game__component__LifeParam.gyml")) {
                std::string currentFilePath = mPath;
                std::shared_ptr<TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component::LifeParam> editor = std::make_shared<TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component::LifeParam>(TotkToolkit::IO::FileHandle(currentFilePath), mPath, nullptr);
                if (editor->Parse())
                    TotkToolkit::UI::EditorSystem::AddEditor(editor);
                else
                    TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::Errors::UI::Items::Windows::Editor::ParseFailed>(currentFilePath));
            }
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("TOTKTOOLKIT_File", this, sizeof(*this));
            ImGui::EndDragDropSource();
        }

        // Add hover effect to the button
        if (ImGui::IsItemHovered()) {
            ImVec2 hoverRectMin = ImVec2(itemScreenPos.x, itemScreenPos.y);
            ImVec2 hoverRectMax = ImVec2(itemScreenPos.x + itemWidth, itemScreenPos.y + itemWidth);
            ImVec4 hoverColor = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];
            ImGui::GetWindowDrawList()->AddRectFilled(hoverRectMin, hoverRectMax, IM_COL32(hoverColor.x * 255, hoverColor.y * 255, hoverColor.z * 255, hoverColor.w * 255), ImGui::GetStyle().FrameRounding);
        }

        // Make right-click context menu appear for button
        if (ImGui::BeginPopupContextItem()) {
            ImGui::PushID("File Context Menu");
            if (ImGui::BeginMenu("Open With")) {
                ImGui::PushID("Open With");

                if (ImGui::MenuItem("BYML Text")) {
                    std::string currentFilePath = mPath;
                    std::shared_ptr<TotkToolkit::UI::Items::Windows::Editors::Texts::BYML> editor = std::make_shared<TotkToolkit::UI::Items::Windows::Editors::Texts::BYML>(TotkToolkit::IO::FileHandle(currentFilePath), mPath, nullptr);
                    if (editor->Parse())
                        TotkToolkit::UI::EditorSystem::AddEditor(editor);
                    else
                        TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::Errors::UI::Items::Windows::Editor::ParseFailed>(currentFilePath));
                }
                else if (ImGui::MenuItem("TXTG Texture")) {
                    std::string currentFilePath = mPath;
                    std::shared_ptr<TotkToolkit::UI::Items::Windows::Editors::TXTG> editor = std::make_shared<TotkToolkit::UI::Items::Windows::Editors::TXTG>(TotkToolkit::Resources::TexToGo::GetTXTGByFilepath(currentFilePath), mPath, nullptr);
                    if (editor->Parse())
                        TotkToolkit::UI::EditorSystem::AddEditor(editor);
                    else
                        TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::Errors::UI::Items::Windows::Editor::ParseFailed>(currentFilePath));
                }

                ImGui::PopID();
                ImGui::EndMenu();
            }

            ImGui::PopID();
            ImGui::EndPopup();
        }

        // Set the cursor back as if we never drew that invisible button
        ImGui::SetCursorPos(itemPos);

        // Draw the icon
        ImGui::PushFont(TotkToolkit::UI::Fonts::sNormalFont2x);
        TotkToolkit::UI::ImGuiUtil::TextCentered(TotkToolkit::UI::Icons::FILE_ICON, ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + itemWidth);
        ImGui::PopFont();

        // Draw the name
        ImGui::PushFont(TotkToolkit::UI::Fonts::sNormalFont_75x);
        TotkToolkit::UI::ImGuiUtil::TextCenteredWrapped(filename.c_str(), ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + itemWidth);
        ImGui::PopFont();

        ImGui::EndGroup();
	}
}