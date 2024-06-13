#pragma once

#include <TotkToolkit/Resources/BGYMLs/game/component/LifeParam.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>
#include <Formats/Resources/BYML/Nodes/String.h>

namespace TotkToolkit::Resources::BGYMLs::game::component {
	LifeParam::LifeParam(TotkToolkit::IO::FileHandle fileHandle) : TotkToolkit::Resources::BGYML(fileHandle) {
	}

	bool LifeParam::Parse() {
		if (!TotkToolkit::Resources::BGYML::Parse())
			return false;

		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		if (stringHash == nullptr)
			return false;

		if (stringHash->HasKey("DamageParameters")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["DamageParameters"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = node->AsString();
			mDamageParametersPath = string->AsStdString();
		}
		if (stringHash->HasKey("LifeParameters")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["LifeParameters"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = node->AsString();
			mLifeParametersPath = string->AsStdString();
		}

		if (stringHash->HasKey("InitInvincibilityType")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["InitInvincibilityType"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = node->AsString();
			mInitInvincibilityType = string->AsStdString();
		}

		return true;
	}

	bool LifeParam::Serialize() {
		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		(*stringHash)["DamageParameters"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mDamageParametersPath));
		(*stringHash)["LifeParameters"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mLifeParametersPath));
		(*stringHash)["InitInvincibilityType"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mInitInvincibilityType));

		std::shared_ptr<Formats::IO::Stream> writeStream = mFileHandle.OpenWriteStream();

		TotkToolkit::Resources::BGYML::SetStream(writeStream);
		if (!TotkToolkit::Resources::BGYML::Serialize())
			return false;
		writeStream->Flush();

		return true;
	}

	std::string LifeParam::GetDamageParametersPath() {
		return mDamageParametersPath;
	}
	void LifeParam::SetDamageParametersPath(std::string damageParametersPath) {
		mDamageParametersPath = damageParametersPath;
	}

	std::string LifeParam::GetLifeParametersPath() {
		return mLifeParametersPath;
	}
	void LifeParam::SetLifeParametersPath(std::string lifeParametersPath) {
		mLifeParametersPath = lifeParametersPath;
	}

	std::string LifeParam::GetInitInvincibilityType() {
		return mInitInvincibilityType;
	}
	void LifeParam::SetInitInvincibilityType(std::string initInvincibilityType) {
		mInitInvincibilityType = initInvincibilityType;
	}
}