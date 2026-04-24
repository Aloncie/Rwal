#pragma once
#include "logs/logs.hpp"

#include <string>
#include <filesystem>
#include <QDir>
#include <optional>

namespace fs = std::filesystem;

class AppSettings{
public:
	bool isCurcorVisible() const;
	void switchCursorVisiable();
};

class LinuxSystemScheduler : public ISystemScheduler{
private:
	std::optional<fs::path> getUserTimerPath() const;
	Logs& m_logs;
public:
	Timer(Logs& logs) : m_logs(logs) override {}
	bool createScheduler() override;
	bool reload() override;
	std::string get() const override;
	bool set(const std::string& value) override;
	bool status() const override;

	bool check_timer_mask_status();
};

