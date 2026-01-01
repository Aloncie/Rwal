#pragma once
#include <vector>
#include <string>
#include <functional>
#include <optional>

struct MenuConfig{
	const std::string valid_choices;
	std::function<std::vector<std::string>()> menu;
};

namespace rwal::ui {
	namespace generators {
		std::vector<std::string> keywords_list();
		std::vector<std::string> setting_list();
	}
	namespace ready {
		const std::vector<std::string> main_list();
		const std::vector<std::string> timer_list();
	}
}

extern const MenuConfig MAIN_MENU;
extern const MenuConfig KEYWORDS_MENU; 
extern const MenuConfig SETTINGS_MENU;
extern const MenuConfig TIMER_MENU;
