#pragma once

#include <TotkToolkit/Resource.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <TotkTOolkit/Resources/BYML.h>
#include <Formats/Resources/BYML/BYML.h>
#include <string>
#include <memory>

namespace TotkToolkit::Resources::game::component {
	class LifeParam : virtual public TotkToolkit::Resources::BYML {
	public:
		LifeParam(TotkToolkit::IO::FileHandle fileHandle);

		virtual bool Parse() override;
		virtual bool Parse_() override;
		virtual bool Serialize() override;

	protected:
		std::string mParent;
		std::string mDamageParameters;
		std::string mLifeParameters;

		std::string mInitInvincibilityType;
	};
}