#pragma once

#include <TotkToolkit/Resources/game/component/LifeParam.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>
#include <Formats/Resources/BYML/Nodes/String.h>

namespace TotkToolkit::Resources::game::component {
	LifeParam::LifeParam(TotkToolkit::IO::FileHandle fileHandle) : TotkToolkit::Resource(fileHandle), TotkToolkit::Resources::BYML(fileHandle) {
	}

	bool LifeParam::Parse() {
		return Parse_();
	}
	bool LifeParam::Parse_() {
		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::StringHash>(mBYML->GetRoot());
		if (stringHash == nullptr)
			return false;

		if (stringHash->HasKey("$parent")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["$parent"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::String>(node);
			mParent = string->AsStdString();
		}
		if (stringHash->HasKey("DamageParameters")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["DamageParameters"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::String>(node);
			mDamageParameters = string->AsStdString();
		}
		if (stringHash->HasKey("LifeParameters")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["LifeParameters"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::String>(node);
			mLifeParameters = string->AsStdString();
		}

		if (stringHash->HasKey("InitInvincibilityType")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["InitInvincibilityType"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::String>(node);
			mInitInvincibilityType = string->AsStdString();
		}
	}

	bool LifeParam::Serialize() {
		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = std::dynamic_pointer_cast<Formats::Resources::BYML::Nodes::StringHash>(mBYML->GetRoot());
		(*stringHash)["$parent"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mParent));
		(*stringHash)["DamageParameters"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mDamageParameters));
		(*stringHash)["LifeParameters"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mLifeParameters));
		(*stringHash)["InitInvincibilityType"] = mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(mInitInvincibilityType));

		mBYML->SetStream(mFileHandle.GetWriteStream());
		return mBYML->Serialize();
	}
}