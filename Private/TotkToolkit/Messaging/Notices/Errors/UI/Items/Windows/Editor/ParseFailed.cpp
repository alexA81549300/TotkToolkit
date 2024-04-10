#include <TotkToolkit/Messaging/Notices/Errors/UI/Items/Windows/Editor/ParseFailed.h>

#include <TotkToolkit/UI/Localization/TranslationSource.h>

namespace TotkToolkit::Messaging::Notices::Errors::UI::Items::Windows::Editor {
    ParseFailed::ParseFailed(std::string filepath) : TotkToolkit::Messaging::Notices::Error(TotkToolkit::Messaging::NoticeType::ERRORS_UI_WINDOWS_EDITOR_PARSEFAILED, TotkToolkit::UI::Localization::TranslationSource::GetText("PARSE_FAILED") + std::string(": ") + filepath) {

    }
}