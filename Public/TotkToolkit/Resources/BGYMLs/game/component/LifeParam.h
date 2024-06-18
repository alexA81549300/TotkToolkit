#pragma once

#include <TotkToolkit/Resource.h>

#include <TotkToolkit/IO/FileHandle.h>
#include <TotkToolkit/Resources/BGYML.h>
#include <string>
#include <memory>

namespace TotkToolkit::Resources::BGYMLs::game::component {
	class LifeParam : public TotkToolkit::Resources::BGYML {
	public:
		LifeParam(TotkToolkit::IO::FileHandle fileHandle);

		virtual bool Parse() override;
		virtual bool Serialize() override;

		std::string GetBlackboardTableRefPath();
		void SetBlackboardTableRefPath(std::string blackboardTableRefPath);

		std::string GetDamageParametersPath();
		void SetDamageParametersPath(std::string damageParameters);

		std::string GetLifeParametersPath();
		void SetLifeParametersPath(std::string lifeParametersPath);

		std::string GetInitInvincibilityType();
		void SetInitInvincibilityType(std::string initInvincibilityType);

	protected:
		std::string mBlackboardTableRefPath;
		std::string mDamageParametersPath;
		std::string mLifeParametersPath;
		std::string mInitInvincibilityType;
	};
}