#pragma once
#include <string>
#include <exception>
#include <filesystem>
#include "logs/logs.h"

void save_wallpaper(std::string from);
void refresh_wallpaper(int argc, char *argv[],int& count);
std::string where_are_wallpaper();
