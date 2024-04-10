#pragma once

#include <TotkToolkit/UI/Items/Window.h>

#include <string>

namespace TotkToolkit::UI::Items::Windows::Configuration {
    class Settings : public TotkToolkit::UI::Items::Window {
    public:
        Settings(bool* open);

        virtual void DrawContents() override;
    };
}