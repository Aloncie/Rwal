#include "menus.hpp"
#include "settings/settings.hpp"
#include "ui/cli/cli.hpp"
#include "keywords/keywords.hpp"
#include "settings/config.hpp"
#include "internal/utils/string_utils.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include <QCoreApplication>

namespace rwal::ui {

	const CharacterMenuConfig MAIN_MENU = {
		"1234q",                     
		ready::main_list,  
		[](std::string input) -> MenuResponce { 
			if (input == "1") { 
				refresh_wallpaper(); 
				return {nullptr, false, false}; 
			} else if (input == "2") { 
				save_wallpaper(where_are_wallpaper()); 
				return {nullptr, false, false}; 
			} else if (input == "3") {
				return {&KEYWORDS_MENU, false, false};
			} else if (input == "4") {
				return {&SETTINGS_MENU, false, false};
			} else if (input == "q") {
				return {nullptr, false, true};
			} else {
				return {nullptr, true, false};
			}
		}
	};

	const CharacterMenuConfig TIMER_MENU = {
		"nhd",
		ready::timer_list,
		[](std::string input) -> MenuResponce {
			Timer t;
			if (input == "h") {
				t.edit_timer("hourly");
				return {&SETTINGS_MENU, false, false};
			} else if (input == "d") {
				t.edit_timer("daily");
				return {&SETTINGS_MENU, false, false};
			} else if (input == "n") {
				t.edit_timer("None");
				return {&SETTINGS_MENU, false, false};
			} else {
				return {nullptr, true, false};
			}
		}
	};
	const CharacterMenuConfig KEYWORDS_MENU = {
		"armq", 
		generators::keywords_list,
		[](std::string input) -> MenuResponce {
			Keywords k;
			std::vector<std::string> keywords = k.ShortWayGetKeywords();

			if (input == "a") { 
				std::string keyword = MenuManager::getInstance().request_input<std::string>("Write new keyword: ");
				if (!keyword.empty()) {
					rwal::utils::string::format(keyword); 
					keywords.push_back(keyword);
					Config::getInstance().set("/search/keywords", keywords);
				}
				return {nullptr, false, false};
			} else if (input == "r") {
				int display_index = MenuManager::getInstance().request_input<int>("Enter index for remove: ");
				if (display_index >= 1) {
					int real_index = display_index - 1;
					if (real_index < (int)keywords.size()) {
						keywords.erase(keywords.begin() + real_index);
						Config::getInstance().set("/search/keywords", keywords);
					}
				}
				return {nullptr, false, false};
			} else if (input == "m") {
				k.editKeywords();
				Config::getInstance().loadConfig();
				return {nullptr, false, false};
			} else if (input == "q") {
				return {&MAIN_MENU, false, false};
			} else {
				return {nullptr, true, false};
			}
		}
	};

	const CharacterMenuConfig SETTINGS_MENU = {
		"12q",
		generators::settings_list,
		[](std::string input) -> MenuResponce {
			if (input == "1") {
				return {&TIMER_MENU, false, false};
			} else if (input == "2") {
				return {nullptr, false, false}; // Место для логики пути
			} else if (input == "q") {
				return {&MAIN_MENU, false, false};
			} else {
				return {nullptr, true, false};
			}
		}
	};

	namespace generators {
		std::vector<std::string> keywords_list() {
			std::vector<std::string> lines = {"--- Keywords Editor ---"};
			Keywords k;
			auto keywords = k.ShortWayGetKeywords();
			if (keywords.empty()) lines.push_back("None"); 
			else {
				for(size_t i = 0; i < keywords.size(); ++i)
					lines.push_back(std::to_string(i + 1) + ". " + keywords[i]);
			}
			lines.push_back("a) Add | r) Remove | m) Manual(editor) | q) Back");
			return lines;
		}
		std::vector<std::string> settings_list() {
			Timer t;
			return {
				"--- Settings ---",
				"1) Timer: " + t.see_timer(),
				"2) Wallpapers's path: " + where_are_wallpaper(),
				"q) Back"
			};
		}
	}

	namespace ready {
		std::vector<std::string> main_list() {
			return {"--- Main Menu ---", "1) Refresh", "2) Save", "3) Keywords", "4) Settings", "q) Quit"};
		}
		std::vector<std::string> timer_list() {
			return {"(n)one", "(h)ourly", "(d)aily"};
		}
	}

}
