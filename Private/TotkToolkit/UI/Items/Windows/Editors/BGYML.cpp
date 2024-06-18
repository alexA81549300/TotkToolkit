#include <TotkToolkit/UI/Items/Windows/Editors/BGYML.h>

#include <TotkToolkit/IO/Filesystem.h>
#include <filesystem>

namespace TotkToolkit::UI::Items::Windows::Editors {
	BGYML::BGYML(TotkToolkit::IO::FileHandle fileHandle, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(fileHandle, name, open) {

	}
	BGYML::BGYML(std::shared_ptr<TotkToolkit::Resources::BGYML> resource, std::string name, bool* open) : TotkToolkit::UI::Items::Windows::Editor(resource, name, open) {

	}

	std::string BGYML::ConvertPathToReal(std::string filepath) {
		if (TotkToolkit::IO::Filesystem.FileExists(filepath))
			return filepath;
		std::filesystem::path path = filepath;

		std::string newFilepath = filepath;
		if (path.extension().generic_string() == ".gyml")
			newFilepath = path.replace_extension(".bgyml").generic_string();
		if (TotkToolkit::IO::Filesystem.FileExists(newFilepath))
			return newFilepath;

		return filepath;
	}
	std::string BGYML::ConvertPathToVirtual(std::string filepath) {
		if (TotkToolkit::IO::Filesystem.FileExists(filepath))
			return filepath;
		std::filesystem::path path = filepath;
		std::string extension = path.extension().generic_string();
		if (extension == ".bgyml")
			extension = ".gyml";
		std::string newFilepath = path.replace_extension(extension).generic_string();
		if (TotkToolkit::IO::Filesystem.FileExists(newFilepath))
			return newFilepath;

		return filepath;
	}
}