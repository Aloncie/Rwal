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
	void setup_systemd_timer();
	std::string see_timer();
	void edit_timer();
};
