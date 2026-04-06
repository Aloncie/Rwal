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

class Timer{
private:
	std::optional<fs::path> getUserTimerPath() const;
	Logs& m_logs;
public:
	Timer(Logs& logs) : m_logs(logs) {}
	void createSystemdTimer();
	std::string seeTimer();
	std::string editTimer(std::string value);
	bool TimerStatus();
	bool check_timer_mask_status();
};

