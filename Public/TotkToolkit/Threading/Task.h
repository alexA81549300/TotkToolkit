#pragma once

#include <TotkToolkit/Threading/TaskReport.h>
#include <TotkToolkit/Threading/TaskType.h>
#include <functional>
#include <future>
#include <memory>
#include <atomic>

namespace TotkToolkit::Threading {
    class Task {
    public:
        Task(std::function<void()> callback, std::shared_ptr<TotkToolkit::Threading::TaskReport> taskReport, TotkToolkit::Threading::TaskType type);

        TotkToolkit::Threading::TaskType GetType();

        std::shared_ptr<TotkToolkit::Threading::TaskReport> GetTaskReport();

        virtual void Execute() = 0;
        void ExecuteAsync();
        void Abort();
    protected:
        const std::shared_ptr<TotkToolkit::Threading::TaskReport> mTaskReport; // TOTKTOOLKIT_CONSIDERATION_THREAD_SAFETY: Does not need to be marked atomic because it is initialized when the object is constructed on the calling thread, and then not mutated.
        std::function<void()> mCallback;
        std::future<void> mFuture;
        std::shared_ptr<std::atomic<bool>> mContinueCondition;
        TotkToolkit::Threading::TaskType mType;
    };
}