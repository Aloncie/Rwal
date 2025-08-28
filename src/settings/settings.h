#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

fs::path get_pictures_path();

class Timer{
public:
	void create_systemd_timer();
	std::string see_timer();
	std::string edit_timer(std::string value);
	bool check_timer_active_status();
	bool check_timer_mask_status();
};
