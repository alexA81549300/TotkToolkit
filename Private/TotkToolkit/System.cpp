#include <TotkToolkit/System.h>

#include <Formats/Aliases/Primitives.h>
#include <algorithm>

namespace TotkToolkit {
	std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> System::GetTaskReports() {
		std::shared_lock<std::shared_mutex> lock(mTasksMutex);

		std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> res;
		std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> tasks = GetTasks();
		res.reserve(tasks.size());
		for (std::shared_ptr<TotkToolkit::Threading::Task> task : tasks) {
			res.push_back(task->GetTaskReport());
		}
		
		return res;
	}

	std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> System::GetTaskReports(TotkToolkit::Threading::TaskType type) {
		std::shared_lock<std::shared_mutex> lock(mTasksMutex);

		std::vector<std::shared_ptr<TotkToolkit::Threading::TaskReport>> res;
		std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> tasks = GetTasks(type);
		res.reserve(tasks.size());
		for (std::shared_ptr<TotkToolkit::Threading::Task> task : tasks) {
			res.push_back(task->GetTaskReport());
		}

		return res;
	}

	std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> System::GetTasks() {
		std::shared_lock<std::shared_mutex> lock(mTasksMutex);

		std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> res;
		res.reserve(mTasks.size()); // Probably reasonable. Oftentimes there won't be that many tasks of the same type.
		for (auto& it : mTasks) {
			for (std::shared_ptr<TotkToolkit::Threading::Task> task : it.second) {
				res.push_back(task);
			}
		}

		return res;
	}
	std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> System::GetTasks(TotkToolkit::Threading::TaskType type) {
		std::shared_lock<std::shared_mutex> lock(mTasksMutex);

		std::vector<std::shared_ptr<TotkToolkit::Threading::Task>> res;
		if (!mTasks.contains(type))
			return res;
		res.reserve(mTasks.at(type).size());
		for (std::shared_ptr<TotkToolkit::Threading::Task> task : mTasks.at(type)) {
			res.push_back(task);
		}

		return res;
	}

	bool System::AddTask(std::shared_ptr<TotkToolkit::Threading::Task> task) {
		std::unique_lock<std::shared_mutex> lock(mTasksMutex);

		if (mTasks.contains(task->GetType())) {
			if (std::find(mTasks.at(task->GetType()).begin(), mTasks.at(task->GetType()).end(), task) != mTasks.at(task->GetType()).end())
				return false; // Already have this task!
			mTasks.at(task->GetType()).push_back(task);
		}
		else {
			mTasks.insert({ task->GetType(), std::vector<std::shared_ptr<TotkToolkit::Threading::Task>>({ task }) });
		}
		return true;
	}

	bool System::RemoveTask(std::shared_ptr<TotkToolkit::Threading::Task> task) {
		std::unique_lock<std::shared_mutex> lock(mTasksMutex);
		if (!mTasks.contains(task->GetType()))
			return false;

		if (std::find(mTasks.at(task->GetType()).begin(), mTasks.at(task->GetType()).end(), task) == mTasks.at(task->GetType()).end())
			return false;

		mTasks.at(task->GetType()).erase(std::remove(mTasks.at(task->GetType()).begin(), mTasks.at(task->GetType()).end(), task), mTasks.at(task->GetType()).end());
	}
}