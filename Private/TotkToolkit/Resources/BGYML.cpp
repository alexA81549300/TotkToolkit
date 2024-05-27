#include <TotkToolkit/Resources/BGYML.h>

namespace TotkToolkit::Resources {
	BGYML::BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::Resource(fileHandle) {

	}

	bool BGYML::Parse() {
		return Parse_();
	}
	bool BGYML::Parse_() {
		mBYML = Formats::Resources::BYML::BYML::Factory(mFileHandle.OpenReadStream());
		if (mBYML == nullptr)
			return false;

		return true;
	}

	bool BGYML::Serialize() {
		mBYML->SetStream(mFileHandle.OpenWriteStream());
		return mBYML->Serialize();
	}
}