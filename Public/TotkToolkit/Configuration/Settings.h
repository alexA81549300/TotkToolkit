#pragma once

#include <string>

namespace TotkToolkit::Configuration {
	class Settings {
	public:
		static void Init();

		static std::string GetDumpDir();
		static void SetDumpDir(std::string dumpDir);

		static std::string GetBfresDir();
		static void SetBfresDir(std::string bfresDir);

		static std::string GetWriteDir();
		static void SetWriteDir(std::string writeDir);

	protected:
		static void Load();
		static void Save();

		static bool _SetDumpDir(std::string dumpDir);

		static bool _SetBfresDir(std::string bfresDir);
	    
		static bool _SetWriteDir(std::string writeDir);

		static std::string sDumpDir;
		static std::string sBfresDir; // Unfortunately MeshCodec isn't fully reverse-engineered yet.
		static std::string sWriteDir;
	};
}