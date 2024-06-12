#include <TotkToolkit/UI/Items/Windows/Filesystem/Browser.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <TotkToolkit/IO/Filesystem.h>
#include <TotkToolkit/UI/MainWindow.h>
#include <TotkToolkit/UI/EditorSystem.h>
#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <TotkToolkit/UI/ImGuiUtil.h>
#include <TotkToolkit/UI/Icons.h>
#include <TotkToolkit/UI/Fonts.h>
#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/DumpDir.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/Romfs.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/WriteDir.h>
#include <TotkToolkit/Messaging/Notices/Errors/UI/Items/Windows/Editor/ParseFailed.h>
#include <Formats/Aliases/Primitives.h>
#include <Fallback/shared_ptr_atomic.h>
#include <imgui.h>
#include <future>

namespace TotkToolkit::UI::Items::Windows::Filesystem {
    Browser::Browser(bool* open) : TotkToolkit::UI::Items::Window(TotkToolkit::UI::Localization::TranslationSource::GetText("BROWSER"), open) {
        TotkToolkit::Messaging::NoticeBoard::AddReceiver(this);

        std::unique_lock<std::shared_mutex> lock(mSegmentedCurrentPathMutex);
        mSegmentedCurrentPath.push_back("Work");
    }

    void Browser::DrawContents() {
        std::unique_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock1(mSegmentedCurrentPathMutex);
        std::shared_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock2(mCurrentFilesMutex);
        std::shared_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock3(mCurrentDirectoriesMutex);


        std::shared_ptr<TotkToolkit::Threading::TaskReport> filesystemLoadTaskReport = TotkToolkit::IO::Filesystem.GetLoadTaskReport();
        if (filesystemLoadTaskReport != nullptr && !filesystemLoadTaskReport->IsFinished()) {
            ImGui::ProgressBar(filesystemLoadTaskReport->GetProgress());
        }
        for (F_U32 i = 0; i < mSegmentedCurrentPath.size(); i++) {
            ImGui::AlignTextToFramePadding();
            ImVec2 folderNameStartPos = ImGui::GetCursorPos();
            ImGui::TextDisabled(mSegmentedCurrentPath.at(i).c_str());
            ImVec2 folderNameEndPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(folderNameStartPos);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
            if (ImGui::Button(AppendIdentifier("", "Path Button " + std::to_string(i)).c_str(), ImGui::GetItemRectSize())) {
                ImGui::PopStyleColor();
                mSegmentedCurrentPath.erase(mSegmentedCurrentPath.begin() + i + 1, mSegmentedCurrentPath.end());
                UpdateItems();
            }
            else
                ImGui::PopStyleColor();
            ImGui::SetCursorPos(folderNameEndPos);

            ImGui::SameLine();
            ImGui::TextDisabled("/");
            ImGui::SameLine();
        }

        ImGui::NewLine();

        float itemWidth = 4 * ImGui::GetFontSize();
        int itemsPerCol = (ImGui::GetContentRegionAvail().x / itemWidth) - 1;
        ImVec2 fileTableStart = ImGui::GetCursorPos();
        ImVec2 fileTableSize = ImGui::GetContentRegionAvail();
        if (itemsPerCol > 0) {
            if (ImGui::BeginTable(AppendIdentifier("File Table").c_str(), itemsPerCol, ImGuiTableFlags_SizingFixedFit)) {
                for (F_U32 i = 0; i < mCurrentDirectories.size(); i++) {
                    // Go to the next column
                    ImGui::TableNextColumn();

                    // Get the position we're at
                    ImVec2 itemPos = ImGui::GetCursorPos();
                    ImVec2 itemScreenPos = ImGui::GetCursorScreenPos();
                    
                    // Don't draw the item if it isn't in view
                    if (itemPos.y + itemWidth < ImGui::GetScrollY()) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemWidth);
                        continue;
                    }
                    if (itemPos.y > fileTableStart.y + ImGui::GetScrollY() + fileTableSize.y) {
                        // Just keep moving the cursor forward so it can still scroll
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemWidth);
                        continue;
                    }

                    // Make the actual button the user can click on
                    if (ImGui::InvisibleButton(("Directory " + mCurrentDirectories[i]).c_str(), ImVec2(itemWidth, itemWidth))) {
                        mSegmentedCurrentPath.push_back(mCurrentDirectories[i]);
                        UpdateItems();
                        break;
                    }

                    // Add hover effect to the button
                    if (ImGui::IsItemHovered()) {
                        ImVec2 hoverRectMin = ImVec2(itemScreenPos.x, itemScreenPos.y);
                        ImVec2 hoverRectMax = ImVec2(itemScreenPos.x + itemWidth, itemScreenPos.y + itemWidth);
                        ImVec4 hoverColor = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];
                        ImGui::GetWindowDrawList()->AddRectFilled(hoverRectMin, hoverRectMax, IM_COL32(hoverColor.x * 255, hoverColor.y * 255, hoverColor.z * 255, hoverColor.w * 255), ImGui::GetStyle().FrameRounding);
                    }

                    // Set the cursor back as if we never drew that invisible button
                    ImGui::SetCursorPos(itemPos);

                    // Draw the icon
                    ImGui::PushFont(TotkToolkit::UI::Fonts::sNormalFont2x);
                    TotkToolkit::UI::ImGuiUtil::TextCentered(TotkToolkit::UI::Icons::FOLDER_ICON, ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + itemWidth);
                    ImGui::PopFont();

                    // Draw the name
                    ImGui::PushFont(TotkToolkit::UI::Fonts::sNormalFont_75x);
                    TotkToolkit::UI::ImGuiUtil::TextCenteredWrapped((mCurrentDirectories[i]).c_str(), ImGui::GetCursorPos().x, ImGui::GetCursorPos().x + itemWidth);
                    ImGui::PopFont();
                }
                for (F_U32 i = 0; i < mCurrentFiles.size(); i++) {
                    ImGui::PushID(i);

                    // Go the the next column
                    ImGui::TableNextColumn();

                    // Get the position we're at
                    ImVec2 itemPos = ImGui::GetCursorPos();
                    ImVec2 itemScreenPos = ImGui::GetCursorScreenPos();

                    // Don't draw the item if it isn't in view
                    if (itemPos.y + itemWidth < ImGui::GetScrollY()) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemWidth);
                        ImGui::PopID();
                        continue;
                    }
                    if (itemPos.y > fileTableStart.y + ImGui::GetScrollY() + fileTableSize.y) {
                        // Just keep moving the cursor forward so it can still scroll
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemWidth);
                        ImGui::PopID();
                        continue;
                    }

                    mCurrentFiles[i].Draw();

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
    }

    std::string Browser::GetCurrentPath() {
        std::shared_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock(mSegmentedCurrentPathMutex);
        std::string res = "";

        for (std::string segment : mSegmentedCurrentPath)
            res += segment + "/";

        return res;
    }

    void Browser::HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) {
        switch (notice->mType) {
            case TotkToolkit::Messaging::NoticeType::IO_FILESYSTEM_FILESCHANGE: {
                std::shared_ptr<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange> castNotice = std::static_pointer_cast<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>(notice);

                TotkToolkit::IO::Filesystem.InitThread();
                TotkToolkit::IO::Filesystem.SyncThread();
                UpdateItems();
                TotkToolkit::IO::Filesystem.DeinitThread();
                return;
            }
            default:
                return;
        }
    }

    void Browser::UpdateItems() {
        UpdateFiles();
        UpdateDirectories();
    }

    void Browser::UpdateFiles () {
        TotkToolkit::IO::Filesystem.InitThread();
        TotkToolkit::IO::Filesystem.SyncThread();

        std::string currentPath = GetCurrentPath();
        std::vector<std::string> currentFiles = TotkToolkit::IO::Filesystem.EnumerateFiles(currentPath);

        std::unique_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock(mCurrentFilesMutex);
        mCurrentFiles.resize(currentFiles.size());
        for (int i = 0; i < currentFiles.size(); i++)
            mCurrentFiles[i] = TotkToolkit::UI::Items::Filesystem::File(currentPath + currentFiles[i]);
    }

    void Browser::UpdateDirectories() {
        TotkToolkit::IO::Filesystem.InitThread();
        TotkToolkit::IO::Filesystem.SyncThread();

        std::vector<std::string> currentDirectories = TotkToolkit::IO::Filesystem.EnumerateDirectories(GetCurrentPath());

        std::unique_lock<TotkToolkit::Threading::Mutexes::SharedRecursive> lock(mCurrentDirectoriesMutex);
        mCurrentDirectories = currentDirectories;
    }
}