#include <TotkToolkit/Threading/Tasks/IO/Filesystem/MountArchives.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/IO/Filesystem.h>
#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <Formats/Aliases/Primitives.h>
#include <physfs.h>
#include <filesystem>
#include <vector>
#include <memory>

namespace TotkToolkit::Threading::Tasks::IO::Filesystem {
    MountArchives::MountArchives(std::string dir, std::function<void()> callback, std::function<void(std::string)> floatDirectoryFn, std::function<std::vector<std::string>()> getCachedPathsFn, std::function<void(std::vector<std::string>)> setCachedPathsFn) : TotkToolkit::Threading::Task(callback, std::make_shared<TotkToolkit::Threading::TaskReport>(TotkToolkit::UI::Localization::TranslationSource::GetText("MOUNT_ARCHIVES")), TotkToolkit::Threading::TaskType::IO_FILESYSTEM_MOUNTARCHIVES), mDir(dir), mFloatDirectoryFn(floatDirectoryFn), mGetCachedPathsFn(getCachedPathsFn), mSetCachedPathsFn(setCachedPathsFn) {

    }

    void MountArchives::Execute() {
        mTaskReport->SetMessage(TotkToolkit::UI::Localization::TranslationSource::GetText("MOUNTING_PACKS"));

        TotkToolkit::IO::Filesystem.InitThread();
        TotkToolkit::IO::Filesystem.SyncThread();

        std::vector<std::string> packPaths;

        if (mGetCachedPathsFn != nullptr) {
            packPaths = mGetCachedPathsFn();
        }
        if (packPaths.size() == 0) { // Cache didn't have anything or didn't exist.
            //packPaths = TotkToolkit::IO::Filesystem.SearchFilenamesByExtension("", ".pack.zs", mContinueCondition); // Equivilent to regex search for R"(\.pack\.zs$)" but faster

            for (const auto& file : std::filesystem::recursive_directory_iterator(mDir, std::filesystem::directory_options::follow_directory_symlink)) {
                if (std::filesystem::is_regular_file(file)) {
                    std::string filename = file.path().filename().string();
                    if (filename.ends_with(".pack") || filename.ends_with(".pack.zs")) {
                        packPaths.push_back(file.path().generic_string());
                    }
                }
            }

            //std::vector<std::string> uncompressedPackPaths = TotkToolkit::IO::Filesystem.SearchFilenamesByExtension("", ".pack", mContinueCondition);
            //packPaths.insert(packPaths.end(), uncompressedPackPaths.begin(), uncompressedPackPaths.end());
            if (mSetCachedPathsFn != nullptr)
                mSetCachedPathsFn(packPaths);
        }
        for (std::string packPath : packPaths) {
            if (!*mContinueCondition)
                return;

            //std::shared_ptr<Formats::IO::Stream> pack = TotkToolkit::IO::Filesystem.OpenReadStream(packPath);
            //if (pack != nullptr) {
            //    TotkToolkit::IO::Filesystem.MountStream(pack, std::filesystem::path(packPath).relative_path().generic_string(), "Work", false, true);
            //}
            //PHYSFS_File* file = PHYSFS_openRead(packPath.c_str());
            //TotkToolkit::IO::Filesystem.MountHandle(file, packPath, "Work", false, true);
            TotkToolkit::IO::Filesystem.Mount(packPath, "Work", false, true);
            if (mFloatDirectoryFn != nullptr)
                mFloatDirectoryFn(std::filesystem::path(packPath).relative_path().generic_string());
            mTaskReport->AddProgress(1.f / packPaths.size());
        }

        mTaskReport->DeclareFinished();

        // Since we bypassed the notice on each mount for performance reasons
        TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>());

        mCallback();
    }
}