#pragma once
#include "ISystemScheduler.hpp"
#include "logs/logs.hpp"

#include <string>

class NullSystemScheduler : public ISystemScheduler {
public:
    NullSystemScheduler(Logs& logs, IFileSystem& fs) : ISystemScheduler(logs, fs) {}

    std::string set(const std::string& value) override {
        m_logs.writeLogs(lvl::Info, mod::Config, "Cannot set schedule: no scheduler available");
        return "Unavailable";
    }
    std::string get() const override {
        m_logs.writeLogs(lvl::Info, mod::Config, "No scheduler available on this platform");
        return "None";
    }

protected:
    bool create() override { return false; }
    bool disable() const override { return false; }
    bool status() const override { return false; }
    bool start() const override { return false; }
    bool reload() override { return false; }
};
