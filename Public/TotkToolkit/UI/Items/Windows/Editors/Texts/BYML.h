#pragma once

#include <TotkToolkit/UI/Items/Windows/Editors/Text.h>
#include <TotkToolkit/Resources/BYML.h>

#include <Formats/Resources/BYML/BYML.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
    class BYML : public TotkToolkit::UI::Items::Windows::Editors::Text, public TotkToolkit::Resources::BYML {
    public:
        BYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

        virtual bool Parse() override;
        virtual bool Serialize() override;
    };
}