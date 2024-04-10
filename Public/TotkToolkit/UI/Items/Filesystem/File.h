#pragma once

#include <TotkToolkit/UI/Item.h>

#include <string>

namespace TotkToolkit::UI::Items::Filesystem {
	class File : public TotkToolkit::UI::Item {
	public:
		File();
		File(std::string path);

		virtual void Draw() override;

		inline std::string GetPath() { return mPath; }

	protected:
		std::string mPath;
	};
}