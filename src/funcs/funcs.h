#pragma once
#include <string>
#include <vector>
#include <random>
#include <filesystem>
#include <exception>
#include <unistd.h>

int random(int count);
std::filesystem::path get_pictures_path();
std::string rwal_catalog();
void setup_systemd_timer();
