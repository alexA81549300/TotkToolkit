#pragma once

#include <TotkToolkit/IO/PHYSFSCall.h>

#include <string>
#include <vector>

namespace TotkToolkit::IO::PHYSFSCalls {
    class Mount : public TotkToolkit::IO::PHYSFSCall {
    public:
        Mount(std::string path, std::string mountPoint, bool appendToPath, std::vector<std::string> floatDirs, bool notifyFilesChanged = true) : mPath(path), mMountPoint(mountPoint), mAppendToPath(appendToPath), mFloatDirs(floatDirs), mNotifyFilesChanged(notifyFilesChanged) {

        }

        virtual void Execute() override;

    protected:
        std::string mPath;
        std::string mMountPoint;
        bool mAppendToPath;
        std::vector<std::string> mFloatDirs;
        bool mNotifyFilesChanged;
        bool mNotifiedFilesChanged = false;
    };
}