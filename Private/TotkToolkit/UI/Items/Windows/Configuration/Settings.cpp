#include <TotkToolkit/UI/Items/Windows/Configuration/Settings.h>

#include <TotkToolkit/Configuration/Settings.h>
#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace TotkToolkit::UI::Items::Windows::Configuration {
    Settings::Settings(bool* open) : TotkToolkit::UI::Items::Window(TotkToolkit::UI::Localization::TranslationSource::GetText("SETTINGS"), open) {
    }

    void Settings::DrawContents() {
        std::string gameDumpDir = TotkToolkit::Configuration::Settings::GetDumpDir();
        ImGui::InputText(AppendIdentifier(TotkToolkit::UI::Localization::TranslationSource::GetText("GAME_DUMP_DIR")).c_str(), &gameDumpDir);
        TotkToolkit::Configuration::Settings::SetDumpDir(gameDumpDir);

        std::string writeDir = TotkToolkit::Configuration::Settings::GetWriteDir();
        ImGui::InputText(AppendIdentifier(TotkToolkit::UI::Localization::TranslationSource::GetText("WRITE_DIR")).c_str(), &writeDir);
        TotkToolkit::Configuration::Settings::SetWriteDir(writeDir);
    }
}