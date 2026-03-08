#pragma once
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
public:
	void createSystemdTimer();
	std::string seeTimer();
	std::string editTimer(std::string value);
	bool TimerStatus();
	bool check_timer_mask_status();
};
