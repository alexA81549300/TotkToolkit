#pragma once

#include <TotkToolkit/UI/Item.h>

#include <TotkToolkit/UI/Items/Windows/Filesystem/Browser.h>
#include <TotkToolkit/UI/Items/Windows/Configuration/Settings.h>
#include <TotkToolkit/UI/Items/Windows/Configuration/Style.h>
#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/Messaging/Receiver.h>

#include <vector>
#include <memory>

namespace TotkToolkit::UI {
    class MainWindow : public TotkToolkit::UI::Item, public TotkToolkit::Messaging::Receiver {
public:
        MainWindow();

        virtual void Draw() override;

        virtual void HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) override;

        std::vector<std::shared_ptr<TotkToolkit::UI::Items::Window>> mEditors;

protected:
        TotkToolkit::UI::Items::Windows::Filesystem::Browser mBrowser;
        TotkToolkit::UI::Items::Windows::Configuration::Settings mSettings;
        TotkToolkit::UI::Items::Windows::Configuration::Style mStyle;

        bool mBrowserOpen = true;
        bool mSettingsOpen = false;
        bool mStyleOpen = false;
    };
}