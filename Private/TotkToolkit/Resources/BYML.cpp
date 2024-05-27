#include <TotkToolkit/Resources/BYML.h>

#include <imgui.h>

namespace TotkToolkit::Resources {
	BYML::BYML(TotkToolkit::IO::FileHandle fileHandle) : TotkToolkit::Resource(fileHandle) {}
	BYML::BYML(std::shared_ptr<Formats::Resource> resource) : TotkToolkit::Resource(resource) {}

	bool BYML::Parse() {
		return Parse_();
	}
	bool BYML::Parse_() {
		mBYML = Formats::Resources::BYML::BYML::Factory(mFileHandle.OpenReadStream());
		if (mBYML == nullptr)
			return false;

		return true;
	}

	bool BYML::Serialize() {
		mBYML->SetStream(mFileHandle.OpenWriteStream());
		return mBYML->Serialize();
	}
}