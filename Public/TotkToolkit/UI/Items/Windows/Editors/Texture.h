#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/UI/Items/Windows/Editor.h>

#include <Formats/Resource.h>
#include <imgui.h>
#include <string>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors {
    class Texture : public TotkToolkit::UI::Items::Windows::Editor {
    public:
        Texture(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);
        Texture(std::shared_ptr<Formats::Resource> resource, std::string name, bool* open);

        virtual void DrawContents() override;

    protected:
        ImTextureID mTextureID;
    };
}