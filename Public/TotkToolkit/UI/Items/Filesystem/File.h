#pragma once

#include <TotkToolkit/UI/Item.h>

#include <Formats/Aliases/Primitives.h>
#include <string>

namespace TotkToolkit::UI::Items::Filesystem {
	class File : public TotkToolkit::UI::Item {
	public:
		File();
		File(std::string path, bool checkValidity = true);

		virtual void Draw() override;
		static F_F32 GetDefaultWidth();

		void CheckValidity();

		inline std::string GetPath() { return mPath; }

	protected:
		std::string mPath;
		bool mValid;
	};
}