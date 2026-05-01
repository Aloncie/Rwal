#pragma once
#include <string>

namespace rwal::ui::Scheduler {
	enum class TaskScheduleType {
		None,
		Hourly,
		Daily
	};

	inline std::string toString(TaskScheduleType type) {
		switch (type) {
			case SchedulerType::None: return "(n)one";
			case SchedulerType::Hourly: return "(h)ourly";
			case SchedulerType::Daily: return "(d)aily";
			default: return "unknown";
		}
	}
	inline TaskScheduleType toType(const std::string& str) {
        if (str == "hourly") return TaskScheduleType::Hourly;
        if (str == "daily") return TaskScheduleType::Daily;
        return TaskScheduleType::None;
    }
}


