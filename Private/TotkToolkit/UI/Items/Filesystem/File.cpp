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
#include <TotkToolkit/IO/Filesystem.h>
#include <imgui.h>
#include <filesystem>

namespace TotkToolkit::UI::Items::Filesystem {
    File::File() : mPath(""), TotkToolkit::UI::Item(), mValid(true) {

    }
	File::File(std::string path, bool checkValidity) : mPath(path), TotkToolkit::UI::Item(), mValid(true) {
        if (checkValidity)
            CheckValidity();
	}

    void File::Draw() {
        F_F32 width = ImGui::CalcItemWidth();
        F_F32 height = 4 * ImGui::GetFontSize();

        if (!mValid)
            ImGui::BeginDisabled();

        ImGui::BeginGroup();

        std::string filename = std::filesystem::path(mPath).filename().string();

        // Get the position we're at
        ImVec2 itemPos = ImGui::GetCursorPos();
        ImVec2 itemScreenPos = ImGui::GetCursorScreenPos();

        // Make the actual button the user can click on
        if (ImGui::InvisibleButton(("File " + mPath).c_str(), ImVec2(width, height))) {
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
            ImVec2 hoverRectMax = ImVec2(itemScreenPos.x + width, itemScreenPos.y + height);
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
        TotkToolkit::UI::ImGuiUtil::TextCentered(TotkToolkit::UI::Icons::FILE_ICON, ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + width);
        ImGui::PopFont();

        // Draw the name
        ImGui::PushFont(TotkToolkit::UI::Fonts::sNormalFont_75x);
        TotkToolkit::UI::ImGuiUtil::TextCenteredWrapped(filename.c_str(), ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + width);
        ImGui::PopFont();

        ImGui::EndGroup();

        if (!mValid)
            ImGui::EndDisabled();
    }
    F_F32 File::GetDefaultWidth() {
        return 4 * ImGui::GetFontSize();
    }

    void File::CheckValidity() {
        TotkToolkit::IO::Filesystem.InitThread();
        TotkToolkit::IO::Filesystem.SyncThread();
        mValid = TotkToolkit::IO::Filesystem.FileExists(mPath);
    }
}