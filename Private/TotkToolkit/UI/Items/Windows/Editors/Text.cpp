#include <TotkToolkit/UI/Items/Windows/Editors/Text.h>

#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace TotkToolkit::UI::Items::Windows::Editors {
    Text::Text(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(fileHandle, name, open) {
        
    }

    void Text::Draw() {
        if (mTextEditor.IsTextChanged()) {
            if (mTextEditor.GetText() != mSavedText)
                mModified = true;
            else
                mModified = false;
        }

        TotkToolkit::UI::Items::Windows::Editor::Draw();
    }

    void Text::DrawContents() {
        TotkToolkit::UI::Items::Windows::Editor::DrawContents();
        
        ImVec2 toolbarCursorStart = ImGui::GetCursorPos();
        // Any sort of toolbar code can be put here.
        ImVec2 toolbarCursorEnd = ImGui::GetCursorPos();

        ImVec2 windowContentRegionSize;
        {
            ImVec2 windowContentRegionMin = ImGui::GetWindowContentRegionMin();
            ImVec2 windowContentRegionMax = ImGui::GetWindowContentRegionMax();

            windowContentRegionSize.x = windowContentRegionMax.x - windowContentRegionMin.x;
            windowContentRegionSize.y = windowContentRegionMax.y - windowContentRegionMin.y;
        }

        ImVec2 toolbarSize = toolbarCursorEnd;
        toolbarSize.x -= toolbarCursorStart.x;
        toolbarSize.y -= toolbarCursorStart.y;



        ImVec2 inputTextSize = windowContentRegionSize;
        inputTextSize.x -= toolbarSize.x;
        inputTextSize.y -= toolbarSize.y;
        //ImGui::InputTextMultiline(Item::AppendIdentifier("##InputText").c_str(), &mText, inputTextSize);
        mTextEditor.Render(Item::AppendIdentifier("##InputText").c_str());
    }
}