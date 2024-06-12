#pragma once

#include <TotkToolkit/Threading/Task.h>
#include <TotkToolkit/Threading/TaskType.h>
#include <map>
#include <vector>
#include <memory>
#include <mutex>

namespace TotkToolkit {
	class System {
	public:
		virtual std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> GetTaskReports();
		virtual std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> GetTaskReports(TotkToolkit::Threading::TaskType type);

	protected:
		virtual std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> GetTasks();
		virtual std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> GetTasks(TotkToolkit::Threading::TaskType type);

		bool AddTask(std::shared_ptr<TotkToolkit::Threading::Task> task);
		bool RemoveTask(std::shared_ptr<TotkToolkit::Threading::Task> task);

		std::map<TotkToolkit::Threading::TaskType, std::vector<std::shared_ptr<TotkToolkit::Threading::Task>>> mTasks; std::shared_mutex mTasksMutex;
	};
}