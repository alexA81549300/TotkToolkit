#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/Messaging/Receiver.h>
#include <TotkToolkit/UI/Items/Filesystem/File.h>
#include <TotkToolkit/Threading/Tasks/IO/Filesystem/MountArchives.h>
#include <TotkToolkit/Threading/Mutexes/SharedRecursive.h>
#include <vector>
#include <string>
#include <memory>
#include <atomic>

namespace TotkToolkit::UI::Items::Windows::Filesystem {
    class Browser : public TotkToolkit::UI::Items::Window, public TotkToolkit::Messaging::Receiver {
    public:
        Browser(bool* open);

        virtual void DrawContents() override;

        std::string GetCurrentPath();

        virtual void HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) override;

    protected:
        void UpdateItems();
        void UpdateFiles();
        void UpdateDirectories();

        std::vector<std::string> mSegmentedCurrentPath; TotkToolkit::Threading::Mutexes::SharedRecursive mSegmentedCurrentPathMutex; // Should always be acquired first to avoid deadlock.
        std::vector<TotkToolkit::UI::Items::Filesystem::File> mCurrentFiles; TotkToolkit::Threading::Mutexes::SharedRecursive mCurrentFilesMutex; // Should always be acquired second to avoid deadlock.
        std::vector<std::string> mCurrentDirectories; TotkToolkit::Threading::Mutexes::SharedRecursive mCurrentDirectoriesMutex; // Should always be acquired third to avoid deadlock.

    };
}