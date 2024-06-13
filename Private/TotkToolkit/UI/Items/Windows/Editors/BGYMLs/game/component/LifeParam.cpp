#include <TotkToolkit/UI/Items/Windows/Editors/BGYMLs/game/component/LifeParam.h>

#include <TotkToolkit/UI/Items/Filesystem/FileHolder.h>
#include <TotkToolkit/UI/Items/Filesystem/File.h>
#include <TotkToolkit/IO/Filesystem.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>
#include <Formats/Resources/BYML/Nodes/String.h>
#include "imgui.h"

namespace TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component {
	LifeParam::LifeParam(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::BGYML(fileHandle, name, open), mLifeParam(std::make_shared <TotkToolkit::Resources::BGYMLs::game::component::LifeParam>(fileHandle)) {
		
	}

	bool LifeParam::Parse() {
		if (!mLifeParam->Parse())
			return false;

		mParentHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mLifeParam->GetParentPath()));
		mDamageParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mLifeParam->GetDamageParametersPath()));
		mLifeParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(mLifeParam->GetLifeParametersPath()));

		return true;
	}
	bool LifeParam::Serialize() {
		mLifeParam->SetParentPath(mParentHolder->GetFile().GetPath());
		mLifeParam->SetDamageParametersPath(mDamageParametersHolder->GetFile().GetPath());
		mLifeParam->SetLifeParametersPath(mLifeParametersHolder->GetFile().GetPath());

		if (!mLifeParam->Serialize())
			return false;

		mModified = false;
		return true;
	}

	void LifeParam::DrawContents() {
		TotkToolkit::UI::Items::Windows::Editors::BGYML::DrawContents();

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
		std::string initInvincibilityType = mLifeParam->GetInitInvincibilityType();
		ImGui::InputText("", initInvincibilityType.data(), 256);
		mLifeParam->SetInitInvincibilityType(initInvincibilityType);
		ImGui::PopID();
	}
}