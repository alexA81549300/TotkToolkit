#include <TotkToolkit/UI/Item.h>

#include <Formats/Aliases/Primitives.h>

namespace TotkToolkit::UI {
    std::string Item::AppendIdentifier(std::string text, std::string additionalIdentification) {
        return text + "##" + additionalIdentification + mId;
    }
    std::string Item::AppendExclusiveIdentifier(std::string text, std::string additionalIdentification) {
        return text + "###" + additionalIdentification + mId;
    }
    std::string Item::GetIdentifier() {
        return mId;
    }

    F_U32 Item::mIdCounter = 0;
}