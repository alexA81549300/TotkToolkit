#include <TotkToolkit/UI/Items/Windows/Editors/BGYMLs/game/component/LifeParam.h>

#include <TotkToolkit/UI/Items/Filesystem/FileHolder.h>
#include <TotkToolkit/UI/Items/Filesystem/File.h>
#include <TotkToolkit/IO/Filesystem.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>
#include <Formats/Resources/BYML/Nodes/String.h>
#include "imgui.h"
#include <filesystem>

namespace TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component {
	LifeParam::LifeParam(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editors::BGYML(fileHandle, name, open), mLifeParam(std::make_shared <TotkToolkit::Resources::BGYMLs::game::component::LifeParam>(fileHandle)) {
		
	}

	bool LifeParam::Parse() {
		if (!mLifeParam->Parse())
			return false;

		mParentHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(ConvertPathToReal(mLifeParam->GetParentPath())));
		mDamageParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(ConvertPathToReal(mLifeParam->GetDamageParametersPath())));
		mLifeParametersHolder = std::make_shared<TotkToolkit::UI::Items::Filesystem::FileHolder>(TotkToolkit::UI::Items::Filesystem::File(ConvertPathToReal(mLifeParam->GetLifeParametersPath())));

		return true;
	}
	bool LifeParam::Serialize() {
		mLifeParam->SetParentPath(ConvertPathToVirtual(mParentHolder->GetFile().GetPath()));
		mLifeParam->SetDamageParametersPath(ConvertPathToVirtual(mDamageParametersHolder->GetFile().GetPath()));
		mLifeParam->SetLifeParametersPath(ConvertPathToVirtual(mLifeParametersHolder->GetFile().GetPath()));

		if (!mLifeParam->Serialize())
			return false;

		mModified = false;
		return true;
	}

	void LifeParam::DrawContents() {
		TotkToolkit::UI::Items::Windows::Editors::BGYML::DrawContents();

		ImGui::PushID(this);

		if (ImGui::BeginTable("Property Table", 2))
		{
			ImGui::TableSetupColumn("Property Name");
			ImGui::TableSetupColumn("Property Value");

			if (mParentHolder) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize("$parent").x);
				ImGui::Text("$parent");

				ImGui::TableSetColumnIndex(1);

				ImGui::PushItemWidth(ImGui::GetColumnWidth());
				mParentHolder->Draw();
				ImGui::PopItemWidth();
			}
			if (mDamageParametersHolder) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize("DamageParameters").x);
				ImGui::Text("DamageParameters");


				ImGui::TableSetColumnIndex(1);

				ImGui::PushItemWidth(ImGui::GetColumnWidth());
				mDamageParametersHolder->Draw();
				ImGui::PopItemWidth();
			}
			if (mLifeParametersHolder) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize("LifeParameters").x);
				ImGui::Text("LifeParameters");


				ImGui::TableSetColumnIndex(1);

				ImGui::PushItemWidth(ImGui::GetColumnWidth());
				mLifeParametersHolder->Draw();
				ImGui::PopItemWidth();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize("InitInvincibilityType").x);
			ImGui::Text("InitInvincibilityType");

			ImGui::TableSetColumnIndex(1);
			std::string initInvincibilityType = mLifeParam->GetInitInvincibilityType();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::PushItemWidth(ImGui::GetColumnWidth());
			ImGui::InputText("##InitInvincibilityTypeInput", initInvincibilityType.data(), 256);
			ImGui::PopItemWidth();
			ImGui::PopStyleVar();
			mLifeParam->SetInitInvincibilityType(initInvincibilityType);

			ImGui::EndTable();
		}

		ImGui::PopID();
	}
}