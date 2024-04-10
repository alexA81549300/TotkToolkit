#pragma once

#include <TotkToolkit/UI/Items/Windows/Editors/Text.h>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
    class Plain : public TotkToolkit::UI::Items::Windows::Editors::Text {
public:
        Plain(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

        virtual bool Parse() override;
        virtual bool Serialize() override;
    };
}