#pragma once

#include <TotkToolkit/Resource.h>

#include <Formats/Resources/BYML/BYML.h>
#include <memory>

namespace TotkToolkit::Resources {
    class BYML : virtual public TotkToolkit::Resource {
    public:
        BYML(TotkToolkit::IO::FileHandle fileHandle);
        BYML(std::shared_ptr<Formats::Resource> resource);

        virtual bool Parse() override;
        virtual bool Parse_() override;
        virtual bool Serialize() override;

    protected:
        std::shared_ptr<Formats::Resources::BYML::BYML> mBYML;
    };
}