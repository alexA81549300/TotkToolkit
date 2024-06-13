#pragma once

#include <TotkToolkit/Resource.h>

#include <memory>

namespace TotkToolkit::Resources {
    class PlainText : virtual public TotkToolkit::Resource {
    public:
        PlainText(TotkToolkit::IO::FileHandle fileHandle);
        PlainText(std::shared_ptr<Formats::Resource> resource);

        virtual bool Parse() override;
        virtual bool Serialize() override;
    };
}