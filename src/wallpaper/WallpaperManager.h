#pragma once
#include <string>

void save_wallpaper(std::string from);
void refresh_wallpaper(int argc, char *argv[],std::string mode = "core", int count = 0);
std::string where_are_wallpaper();
