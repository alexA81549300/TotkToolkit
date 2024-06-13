#include <TotkToolkit/Resources/BGYML.h>

#include <TotkToolkit/IO/Filesystem.h>
#include <Formats/Resources/BYML/Nodes/StringHash.h>

namespace TotkToolkit::Resources {
	BGYML::BGYML(TotkToolkit::IO::FileHandle fileHandle) : TotkToolkit::Resource(fileHandle), mBYML(nullptr), mParent(nullptr), mModified(false) {

	}

	bool BGYML::Parse() {
		mBYML = Formats::Resources::BYML::BYML::Factory(mFileHandle.OpenReadStream());
		if (mBYML == nullptr)
			return false;

		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		if (stringHash == nullptr)
			return false;

		if (stringHash->HasKey("$parent")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["$parent"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = node->AsString();
			mParent = std::make_shared<BGYML>(TotkToolkit::IO::FileHandle(string->AsStdString()));
		}

		return true;
	}

	bool BGYML::Serialize() {
		mBYML->SetStream(mFileHandle.OpenWriteStream());
		return mBYML->Serialize();
	}

	std::string BGYML::GetParentPath() {
		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		if (stringHash == nullptr)
			return "";

		if (stringHash->HasKey("$parent")) {
			std::shared_ptr<Formats::Resources::BYML::Node> node = (*stringHash)["$parent"];
			std::shared_ptr<Formats::Resources::BYML::Nodes::String> string = node->AsString();
			return string->AsStdString();
		}

		return "";
	}
	void BGYML::SetParentPath(std::string parentPath) {
		std::shared_ptr<Formats::Resources::BYML::Nodes::StringHash> stringHash = mBYML->GetRoot()->AsStringHash();
		if (stringHash == nullptr)
			return;

		stringHash->Set("$parent", mBYML->GetStringTable()->GetStringNode(mBYML->GetStringTable()->AddString(parentPath)));

		mParent = std::make_shared<BGYML>(TotkToolkit::IO::FileHandle(parentPath));
	}
}