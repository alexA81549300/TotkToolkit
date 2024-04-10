#pragma once

#include <TotkToolkit/UI/Items/Window.h>
#include <TotkToolkit/Rendering/Framebuffer.h>
#include <glad/glad.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Rendering {
	class Viewport : public TotkToolkit::UI::Items::Window {
public:
		Viewport(std::string name, bool* open);

		virtual void DrawContents() override;

protected:
		std::shared_ptr<TotkToolkit::Rendering::Framebuffer> mFramebuffer;
	};
}