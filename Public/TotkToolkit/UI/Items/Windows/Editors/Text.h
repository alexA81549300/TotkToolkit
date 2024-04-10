#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/UI/Items/Windows/Editor.h>

#include <string>
#include <TextEditor.h>

namespace TotkToolkit::UI::Items::Windows::Editors {
    class Text : public TotkToolkit::UI::Items::Windows::Editor {
public:
        Text(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

        virtual void Draw() override;
        virtual void DrawContents() override;

protected:
        std::string mSavedText;
        ImGuiColorTextEdit::TextEditor mTextEditor;
    };
}