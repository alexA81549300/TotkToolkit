#pragma once

#include <TotkToolkit/Threading/Task.h>
#include <vector>
#include <string>
#include <functional>

namespace TotkToolkit::Threading::Tasks::IO::Filesystem {
    class MountArchives : public TotkToolkit::Threading::Task {
    public:
        MountArchives(std::function<void()> callback = nullptr, std::function<std::vector<std::string>(void)> excludeDirectoriesFn = nullptr, std::function<void(std::string)> floatDirectoryFn = nullptr);

        virtual void Execute() override;

    protected:
        std::function<std::vector<std::string>(void)> mExcludeDirectoriesFn;
        std::function<void(std::string)> mFloatDirectoryFn;
    };
}