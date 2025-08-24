#pragma once
#include <vector>
#include <string>
#include <functional>

struct MenuConfig{
	const std::string valid_choices;
	std::function<std::vector<std::string>()> menu;
};

extern const MenuConfig MAIN_MENU;
extern const MenuConfig KEYWORDS_MENU; 
extern const MenuConfig SETTINGS_MENU;
extern const MenuConfig TIMER_MENU;
