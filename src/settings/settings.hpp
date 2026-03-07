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
	std::optional<fs::path> get_user_timer_path() const;
public:
	void create_systemd_timer();
	std::string see_timer();
	std::string edit_timer(std::string value);
	bool check_timer_active_status();
	bool check_timer_mask_status();
};
