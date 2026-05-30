#pragma once
#include <string>

namespace rwal::system::Scheduler {
	// TaskSchedulerType defines the type of task scheduler
	// Used by every scheduler implementation and SchedulerMenu
	enum class TaskSchedulerType {
		// None = no running task. Don't use if task not found
		None,
		Hourly,
		Daily
	};

	inline std::string toString(TaskSchedulerType type) {
		switch (type) {
			case TaskSchedulerType::None: return "(n)one";
			case TaskSchedulerType::Hourly: return "(h)ourly";
			case TaskSchedulerType::Daily: return "(d)aily";
			default: return "unknown";
		}
	}
	inline TaskSchedulerType toType(const std::string& str) {
        if (str == "hourly") return TaskSchedulerType::Hourly;
        if (str == "daily") return TaskSchedulerType::Daily;
        return TaskSchedulerType::None;
    }
}

