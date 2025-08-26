#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <exception>

namespace fs = std::filesystem;

std::filesystem::path get_pictures_path();
std::string rwal_catalog();

class Timer{
public:
	void create_systemd_timer();
	std::string see_timer();
	std::string edit_timer(std::string value);
	bool check_timer_active_status();
	bool check_timer_mask_status();
};
