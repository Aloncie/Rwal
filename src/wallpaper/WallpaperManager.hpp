#pragma once
#include <string>
#include "ui/cli//UIManager.hpp"
#include "keywords/keywords.hpp"

void save_wallpaper(std::string from, UIManager& ui);
void refresh_wallpaper(Keywords& k, const std::string& mode = "core");
std::string where_are_wallpaper();
