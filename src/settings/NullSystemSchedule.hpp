#pragma once

#include "logs/logs.hpp"
#include <string>

class NullSystemSchedule : public ISystemSchedule {
private:
	Logs& m_logs;
public:
	explicit NullSystemSchedule(Logs& logs) : m_logs(logs) {}

    std::string set(const std::string& value) override {
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Config, "Cannot set schedule: no scheduler available");
		return "Unavailable";
    }
    std::string get() const override {
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Config, "No scheduler available on this platform");
		return "None";
    }
protected:
	bool create() override { return false; }
    bool disable() const override { return false; }
    bool status() const override { return false; }
	bool start() const override { return false; }
	bool reload() override { return false; }
};
