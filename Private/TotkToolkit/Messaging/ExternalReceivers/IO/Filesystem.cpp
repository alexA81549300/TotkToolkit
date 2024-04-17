#include <TotkToolkit/Messaging/ExternalReceivers/IO/Filesystem.h>

#include <TotkToolkit/IO/Filesystem.h>
#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/DumpDir.h>
#include <TotkToolkit/Messaging/Notices/Configuration/Settings/Change/WriteDir.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/Mount/Romfs.h>
#include <Formats/Resources/ZSTD/ZSTDBackend.h>
#include <filesystem>

namespace TotkToolkit::Messaging::ExternalReceivers::IO {
	void Filesystem::HandleNotice(std::shared_ptr<TotkToolkit::Messaging::Notice> notice) {
		switch (notice->mType) {
			case TotkToolkit::Messaging::NoticeType::CONFIGURATION_SETTINGS_CHANGE_DUMPDIR: {
					std::shared_ptr<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::DumpDir> castNotice = std::static_pointer_cast<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::DumpDir>(notice);

					TotkToolkit::IO::Filesystem::InitThread();
					TotkToolkit::IO::Filesystem::SyncThread();
					TotkToolkit::IO::Filesystem::Unmount(castNotice->mOldDumpDir);
					TotkToolkit::IO::Filesystem::Mount((std::filesystem::path(castNotice->mNewDumpDir) / std::filesystem::path("romfs")).string(), "Work");

					// Initialize ZSTD dictionaries
					std::shared_ptr<Formats::IO::Stream> ZsDicPack = TotkToolkit::IO::Filesystem::GetReadStream("Work/Pack/ZsDic.pack.zs");
					if (ZsDicPack != nullptr) {
						TotkToolkit::IO::Filesystem::MountStream(ZsDicPack, "ZsDic.pack.zs", "Work");

						std::shared_ptr<Formats::IO::Stream> ZsZsdic = TotkToolkit::IO::Filesystem::GetReadStream("Work/zs.zsdic");
						if (ZsZsdic != nullptr)
							Formats::Resources::ZSTD::ZSTDBackend::AddDict(ZsZsdic);
						std::shared_ptr<Formats::IO::Stream> BcettBymlZsdic = TotkToolkit::IO::Filesystem::GetReadStream("Work/bcett.byml.zsdic");
						if (BcettBymlZsdic != nullptr)
							Formats::Resources::ZSTD::ZSTDBackend::AddDict(BcettBymlZsdic);
						std::shared_ptr<Formats::IO::Stream> PackZsDic = TotkToolkit::IO::Filesystem::GetReadStream("Work/pack.zsdic");
						if (PackZsDic != nullptr)
							Formats::Resources::ZSTD::ZSTDBackend::AddDict(PackZsDic);
					}

					TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_unique<TotkToolkit::Messaging::Notices::IO::Filesystem::Mount::Romfs>());
					return;
				}
			case TotkToolkit::Messaging::NoticeType::CONFIGURATION_SETTINGS_CHANGE_WRITEDIR: {
					std::shared_ptr<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::WriteDir> castNotice = std::static_pointer_cast<TotkToolkit::Messaging::Notices::Configuration::Settings::Change::WriteDir>(notice);

					TotkToolkit::IO::Filesystem::InitThread();
					TotkToolkit::IO::Filesystem::SyncThread();
					TotkToolkit::IO::Filesystem::SetWriteDir(castNotice->mNewWriteDir);
					return;
				}
			default:
				return;
		}
	}
}