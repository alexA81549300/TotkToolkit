#pragma once

#include <TotkToolkit/IO/PHYSFSCall.h>

#include <string>
#include <vector>

namespace TotkToolkit::IO::PHYSFSCalls {
    class Float : public TotkToolkit::IO::PHYSFSCall {
    public:
        Float(std::vector<std::string> floatDirs) : mFloatDirs(floatDirs) {

        }

        virtual void Execute() override;

    protected:
        std::vector<std::string> mFloatDirs;
    };
}