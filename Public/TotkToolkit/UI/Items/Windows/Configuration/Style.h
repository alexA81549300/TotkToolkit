#pragma once

#include <TotkToolkit/UI/Items/Window.h>

#include <string>

namespace TotkToolkit::UI::Items::Windows::Configuration {
    class Style : public TotkToolkit::UI::Items::Window {
    public:
        Style(bool* open);

        virtual void DrawContents() override;
    };
}