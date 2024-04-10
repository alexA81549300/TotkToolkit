#include <TotkToolkit/Configuration/Settings.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/DumpDir.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/WriteDir.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/BfresDir.h>

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>

namespace TotkToolkit::Configuration {
	void Settings::Init() {
		Load();
	}

	std::string Settings::GetDumpDir() {
		return sDumpDir;
	}
	void Settings::SetDumpDir(std::string dumpDir) {
		if (_SetDumpDir(dumpDir))
			Save();
	}

	std::string Settings::GetBfresDir() {
		return sBfresDir;
	}
	void Settings::SetBfresDir(std::string bfresDir) {
		if (_SetBfresDir(bfresDir))
			Save();
	}

	std::string Settings::GetWriteDir() {
		return sWriteDir;
	}
	void Settings::SetWriteDir(std::string writeDir) {
		if (_SetWriteDir(writeDir))
			Save();
	}

	bool Settings::_SetDumpDir(std::string dumpDir) {
		std::string oldDumpDir = sDumpDir;
		sDumpDir = dumpDir;

		if (oldDumpDir == dumpDir)
			return false;

		TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::DumpDir>(oldDumpDir, dumpDir));

		return true;
	}

	bool Settings::_SetBfresDir(std::string bfresDir) {
		std::string oldBfresDir = sBfresDir;
		sBfresDir = bfresDir;

		if (oldBfresDir == bfresDir)
			return false;

		TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::BfresDir>(oldBfresDir, bfresDir));

		return true;
	}

	bool Settings::_SetWriteDir(std::string writeDir) {
		std::string oldWriteDir = sWriteDir;
		sWriteDir = writeDir;

		if (oldWriteDir == writeDir)
			return false;

		TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::WriteDir>(oldWriteDir, writeDir));

		return true;
	}

	void Settings::Load() {
		if (!std::filesystem::exists("Settings.yml"))
			return;
		YAML::Node root = YAML::LoadFile("Settings.yml");
		_SetDumpDir(root["DumpDir"].Scalar());
		_SetBfresDir(root["BfresDir"].Scalar());
		_SetWriteDir(root["WriteDir"].Scalar());
	}
	void Settings::Save() {
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key;
		emitter << "DumpDir";
		emitter << YAML::Value;
		emitter << GetDumpDir();
		emitter << "BfresDir";
		emitter << YAML::Value;
		emitter << GetBfresDir();
		emitter << "WriteDir";
		emitter << YAML::Value;
		emitter << GetWriteDir();
		emitter << YAML::EndMap;

		std::ofstream stream("Settings.yml");
		stream << emitter.c_str();
	}

	std::string Settings::sDumpDir;
	std::string Settings::sBfresDir;
	std::string Settings::sWriteDir;
}