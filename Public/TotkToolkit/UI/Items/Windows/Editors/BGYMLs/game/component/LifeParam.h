#pragma once

#include <TotkToolkit/UI/Items/Windows/Editors/BGYML.h>
#include <TotkToolkit/Resources/BGYMLs/game/component/LifeParam.h>

#include <TotkToolkit/UI/Items/Filesystem/FileHolder.h>
#include <memory>

namespace TotkToolkit::UI::Items::Windows::Editors::BGYMLs::game::component {
	class LifeParam : public TotkToolkit::UI::Items::Windows::Editors::BGYML {
	public:
		LifeParam(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open);

		virtual bool Parse() override;
		virtual bool Serialize() override;

		virtual void DrawContents() override;

	protected:
		std::shared_ptr<TotkToolkit::Resources::BGYMLs::game::component::LifeParam> mLifeParam;

		std::shared_ptr<TotkToolkit::UI::Items::Filesystem::FileHolder> mParentHolder;
		std::shared_ptr<TotkToolkit::UI::Items::Filesystem::FileHolder> mBlackboardTableRefHolder;
		std::shared_ptr<TotkToolkit::UI::Items::Filesystem::FileHolder> mDamageParametersHolder;
		std::shared_ptr<TotkToolkit::UI::Items::Filesystem::FileHolder> mLifeParametersHolder;
	};
}