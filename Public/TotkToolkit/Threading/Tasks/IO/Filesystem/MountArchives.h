#pragma once

#include <TotkToolkit/Threading/Task.h>
#include <vector>
#include <string>
#include <functional>

namespace TotkToolkit::Threading::Tasks::IO::Filesystem {
    class MountArchives : public TotkToolkit::Threading::Task {
    public:
        MountArchives(std::string dir, std::function<void()> callback = nullptr, std::function<void(std::string)> floatDirectoryFn = nullptr, std::function<std::vector<std::string>()> getCachedPathsFn = nullptr, std::function<void(std::vector<std::string>)> setCachedPathsFn = nullptr);

        virtual void Execute() override;

    protected:
        std::string mDir;
        std::function<void(std::string)> mFloatDirectoryFn;
        std::function<std::vector<std::string>()> mGetCachedPathsFn;
        std::function<void(std::vector<std::string>)> mSetCachedPathsFn;
    };
}