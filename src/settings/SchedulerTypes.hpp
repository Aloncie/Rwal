#pragma once
#include <algorithm>
#include <cctype>
#include <optional>
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

inline std::optional<std::string> toString(TaskSchedulerType type) {
    switch (type) {
    case TaskSchedulerType::None:
        return "none";
    case TaskSchedulerType::Hourly:
        return "hourly";
    case TaskSchedulerType::Daily:
        return "daily";
    default:
        return std::nullopt;
    }
}
inline std::optional<TaskSchedulerType> toType(std::string& str) {
    std::transform(
        str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    if (str == "hourly")
        return TaskSchedulerType::Hourly;
    if (str == "daily")
        return TaskSchedulerType::Daily;
    if (str == "none")
        return TaskSchedulerType::None;
    return std::nullopt;
}
} // namespace rwal::system::Scheduler
