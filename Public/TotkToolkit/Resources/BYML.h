#pragma once

#include <TotkToolkit/Resource.h>

#include <Formats/Resources/BYML/BYML.h>
#include <memory>

namespace TotkToolkit::Resources {
    class BYML : public TotkToolkit::Resource {
    public:
        BYML(TotkToolkit::IO::FileHandle fileHandle);
        BYML(std::shared_ptr<Formats::Resource> resource);

        virtual bool Parse() override;
        virtual bool Serialize() override;

        std::shared_ptr<Formats::Resources::BYML::Nodes::StringTable> GetHashKeyTable() { return mBYML->GetHashKeyTable(); }
        virtual std::shared_ptr<Formats::Resources::BYML::Nodes::StringTable> GetStringTable() { return mBYML->GetStringTable(); }
        virtual std::shared_ptr<Formats::Resources::BYML::Node> GetRoot() { return mBYML->GetRoot(); }

        virtual std::string ToYAML() { return mBYML->ToYAML(); }
        virtual bool LoadYAML(std::string text) { return mBYML->LoadYAML(text); }

    protected:
        std::shared_ptr<Formats::Resources::BYML::BYML> mBYML;
    };
}