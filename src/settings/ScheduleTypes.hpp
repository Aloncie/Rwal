#pragma once
#include <string>

namespace rwal::ui::Schedule {
	// TaskScheduleType defines the type of task schedule
	// Used by every scheduler implementation and ScheduleMenu
	enum class TaskScheduleType {
		// None = no running task. Don't use if task not found
		None,
		Hourly,
		Daily
	};

	inline std::string toString(TaskScheduleType type) {
		switch (type) {
			case TaskScheduleType::None: return "(n)one";
			case TaskScheduleType::Hourly: return "(h)ourly";
			case TaskScheduleType::Daily: return "(d)aily";
			default: return "unknown";
		}
	}
	inline TaskScheduleType toType(const std::string& str) {
        if (str == "hourly") return TaskScheduleType::Hourly;
        if (str == "daily") return TaskScheduleType::Daily;
        return TaskScheduleType::None;
    }
}

