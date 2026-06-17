#pragma once
#include "SchedulerTypes.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/logs.hpp"

#include <optional>
#include <string>

using rwal::system::Scheduler::TaskSchedulerType;

class ISystemScheduler {
public:
    ISystemScheduler(Logs& logs, IFileSystem& fs) : m_logs(logs), m_fs(fs) {};
    virtual ~ISystemScheduler() = default;
    // return string for answer user, get value
    virtual std::string set(TaskSchedulerType type) = 0;
    virtual std::optional<TaskSchedulerType> get() const = 0;

protected:
    Logs& m_logs;
    IFileSystem& m_fs;

    // another methods can be bool only for answer:
    // "How did you do this operation? Success or failure?"
    virtual bool create() = 0;
    virtual bool status() const = 0;
    virtual bool reload() = 0;
    virtual bool start() const = 0;
    virtual bool disable() const = 0;
};
