#pragma once

#include <TotkToolkit/UI/Item.h>

#include <string>
#include <imgui.h>

namespace TotkToolkit::UI::Items {
    class Window : public TotkToolkit::UI::Item {
public:
        Window(std::string name, bool* open);
    
        virtual void Draw() override;
        virtual void DrawContents() {};

        bool IsOpen();

protected:
        std::string mName;
        ImGuiWindowFlags mWindowFlags;
        bool* mOpen;
        bool mOpenLocal;
    };
}