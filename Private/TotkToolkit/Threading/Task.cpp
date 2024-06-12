#include <TotkToolkit/Threading/Task.h>

namespace TotkToolkit::Threading {
    Task::Task(std::function<void()> callback, std::shared_ptr<TotkToolkit::Threading::TaskReport> taskReport, TotkToolkit::Threading::TaskType type) : mCallback(callback), mContinueCondition(std::make_shared<std::atomic<bool>>(true)), mTaskReport(taskReport), mType(type) {

    }

    TotkToolkit::Threading::TaskType Task::GetType() {
        return mType;
    }

    void Task::ExecuteAsync() {
       mContinueCondition = std::make_shared<std::atomic<bool>>(true);
       mFuture = std::async(std::launch::async, [this]() -> void { Execute(); });
    }

    void Task::Abort() {
        *mContinueCondition = false;
    }

    std::shared_ptr<TotkToolkit::Threading::TaskReport> Task::GetTaskReport() {
        return mTaskReport;
    }
}