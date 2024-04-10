#include <TotkToolkit/UI/Items/Window.h>

#include <imgui.h>

namespace TotkToolkit::UI::Items {
    Window::Window(std::string name, bool* open) : mName(name), mWindowFlags(0), mOpen(open), mOpenLocal(true), TotkToolkit::UI::Item() {
        if (open == nullptr)
            mOpen = &mOpenLocal;
    }

    void Window::Draw() {
        ImGui::Begin(AppendIdentifier(mName).c_str(), mOpen, mWindowFlags);
        DrawContents();
        ImGui::End();
    }

    bool Window::IsOpen() {
        return *mOpen;
    }
}