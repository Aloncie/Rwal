#pragma once
#include <string>

enum class TaskSchedulerType {
	None,
	Hourly,
    Daily
};

namespace rwal::ui::Scheduler {
	inline std::string toString(TaskSchedulerType type) {
		switch (type) {
			case SchedulerType::None: return "(n)one";
			case SchedulerType::Hourly: return "(h)ourly";
			case SchedulerType::Daily: return "(d)aily";
			default: return "unknown";
		}
	}
	inline TaskSchedulerType toType(const std::string& str) {
        if (str == "hourly") return TaskSchedulerType::Hourly;
        if (str == "daily") return TaskSchedulerType::Daily;
        return TaskSchedulerType::None;
    }
}


