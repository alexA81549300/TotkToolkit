#include <TotkToolkit/Threading/Tasks/IO/Filesystem/MountArchives.h>

#include <TotkToolkit/Messaging/NoticeBoard.h>
#include <TotkToolkit/Messaging/Notices/IO/Filesystem/FilesChange.h>
#include <TotkToolkit/IO/Filesystem.h>
#include <TotkToolkit/UI/Localization/TranslationSource.h>
#include <Formats/Aliases/Primitives.h>
#include <filesystem>
#include <vector>
#include <memory>

namespace TotkToolkit::Threading::Tasks::IO::Filesystem {
    MountArchives::MountArchives(std::function<void()> callback, std::function<std::vector<std::string>(void)> excludeDirectoriesFn, std::function<void(std::string)> floatDirectoryFn) : TotkToolkit::Threading::Task(callback, std::make_shared<TotkToolkit::Threading::TaskReport>(TotkToolkit::UI::Localization::TranslationSource::GetText("MOUNT_ARCHIVES")), TotkToolkit::Threading::TaskType::IO_FILESYSTEM_MOUNTARCHIVES), mExcludeDirectoriesFn(excludeDirectoriesFn), mFloatDirectoryFn(floatDirectoryFn) {

    }

    void MountArchives::Execute() {
        mTaskReport->SetMessage(TotkToolkit::UI::Localization::TranslationSource::GetText("MOUNTING_PACKS"));

        TotkToolkit::IO::Filesystem.InitThread();
        TotkToolkit::IO::Filesystem.SyncThread();

        std::vector<std::string> excludeDirectories = mExcludeDirectoriesFn != nullptr ? mExcludeDirectoriesFn() : std::vector<std::string>();
        std::vector<std::string> excludeDirectoryMountPoints;
        excludeDirectoryMountPoints.reserve(excludeDirectories.size());
        for (std::string directory : excludeDirectories)
            excludeDirectoryMountPoints.push_back(TotkToolkit::IO::Filesystem.GetMountPoint(directory));
        for (F_U32 i = excludeDirectories.size() - 1; i != -1; i--) {
            if (!TotkToolkit::IO::Filesystem.TempUnmount(excludeDirectories[i])) {
                excludeDirectories.erase(excludeDirectories.begin() + i);
                excludeDirectoryMountPoints.erase(excludeDirectoryMountPoints.begin() + i);
            }
        }

        std::vector<std::string> packPaths = TotkToolkit::IO::Filesystem.SearchFilenamesByExtension("", ".pack.zs", mContinueCondition); // Equivilent to regex search for R"(\.pack\.zs$)" but faster
        packPaths.append_range(TotkToolkit::IO::Filesystem.SearchFilenamesByExtension("", ".pack", mContinueCondition)); // Equivilent to regex search for R"(\.pack\.zs$)" but faster
        for (std::string packPath : packPaths) {
            if (!*mContinueCondition) {
                for (F_U32 i = 0; i < excludeDirectories.size(); i++)
                    TotkToolkit::IO::Filesystem.TempMount(excludeDirectories[i], excludeDirectoryMountPoints[i]);
                return;
            }

            std::shared_ptr<Formats::IO::Stream> pack = TotkToolkit::IO::Filesystem.OpenReadStream(packPath);
            if (pack != nullptr) {
                TotkToolkit::IO::Filesystem.MountStream(pack, std::filesystem::path(packPath).relative_path().generic_string(), "Work", false, true);
            }
            if (mFloatDirectoryFn != nullptr)
                mFloatDirectoryFn(std::filesystem::path(packPath).relative_path().generic_string());
            mTaskReport->AddProgress(1.f / packPaths.size());
        }

        mTaskReport->DeclareFinished();

        // Since we bypassed the notice on each mount for performance reasons
        TotkToolkit::Messaging::NoticeBoard::AddNotice(std::make_shared<TotkToolkit::Messaging::Notices::IO::Filesystem::FilesChange>());

        for (F_U32 i = 0; i < excludeDirectories.size(); i++)
            TotkToolkit::IO::Filesystem.TempMount(excludeDirectories[i], excludeDirectoryMountPoints[i]);

        mCallback();
    }
}