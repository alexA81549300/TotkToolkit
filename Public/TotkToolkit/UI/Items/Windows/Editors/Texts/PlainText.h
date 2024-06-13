#pragma once

#include <TotkToolkit/UI/Items/Windows/Editors/Text.h>

#include <TotkToolkit/Resources/PlainText.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors::Texts {
    class PlainText : public TotkToolkit::UI::Items::Windows::Editors::Text {
    public:
        PlainText(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

        virtual bool Parse() override;
        virtual bool Serialize() override;

    protected:
        std::shared_ptr<TotkToolkit::Resources::PlainText> mPlainText;
    };
}