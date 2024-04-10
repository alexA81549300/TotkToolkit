#include <TotkToolkit/UI/Items/Windows/Editors/BGYMLs/game/component/LifeParam.h>

#include <TotkToolkit/UI/Items/Filesystem/FileHolder.h>
#include <TotkToolkit/UI/Items/Filesystem/File.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>
#include <Formats/Resources/BYML/Nodes/String.h>
#include "imgui.h"

namespace TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component {
	LifeParam::LifeParam(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::BGYML(fileHandle, name, open), TotkToolkit::Resources::game::component::LifeParam(fileHandle), TotkToolkit::Resources::BYML(fileHandle), TotkToolkit::Resource(fileHandle) {
		
	}

	bool LifeParam::Parse() {
		if (!TotkToolkit::Resources::BYML::Parse_())
			return false;
		if (!TotkToolkit::Resources::game::component::LifeParam::Parse_())
			return false;
		if (!TotkToolkit::UI::Items::Windows::Editors::BGYML::Parse_())
			return false;

		return Parse_();
	}
	bool LifeParam::Parse_() {
		mParentHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mParent));
		mDamageParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mDamageParameters));
		mLifeParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mLifeParameters));

		return true;
	}
	bool LifeParam::Serialize() {
		mParent = mParentHolder->GetFile().GetPath();
		mDamageParameters = mDamageParametersHolder->GetFile().GetPath();
		mLifeParameters = mLifeParametersHolder->GetFile().GetPath();

		if (!TotkToolkit::Resources::game::component::LifeParam::Serialize())
			return false;

		mModified = false;
		return true;
	}

	void LifeParam::DrawContents() {
		TotkToolkit::UI::Items::Windows::Editors::BGYML::DrawContents();


		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		if (stringHash == nullptr)
			return;

		if (mParentHolder) {
			ImGui::Text("$parent");
			mParentHolder->Draw();
		}
		if (mDamageParametersHolder) {
			ImGui::Text("DamageParameters");
			mDamageParametersHolder->Draw();
		}
		if (mLifeParametersHolder) {
			ImGui::Text("LifeParameters");
			mLifeParametersHolder->Draw();
		}

		ImGui::PushID("InitInvincibilityType");
		ImGui::Text("InitInvincibilityType");
		ImGui::InputText("", mInitInvincibilityType.data(), 256);
		ImGui::PopID();
	}
}