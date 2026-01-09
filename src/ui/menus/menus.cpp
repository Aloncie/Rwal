#include "menus.h"
#include "settings/settings.h"
#include "ui/cli/cli.h"
#include "keywords/keywords.h"
#include "settings/config.h"
#include "wallpaper/WallpaperManager.h"

namespace rwal::ui {

	const CharacterMenuConfig MAIN_MENU = {
		"1234q",                     
		ready::main_list,  
		[](std::string input) -> MenuResponce { 
			if (input == "1") { 
				refresh_wallpaper(); 
				return {nullptr, false}; 
			} else if (input == "2") { 
				save_wallpaper(where_are_wallpaper()); 
				return {nullptr, false}; 
			} else if (input == "3") {
				return {(BaseMenu*)&KEYWORDS_MENU, false};
			} else if (input == "4") {
				return {(BaseMenu*)&SETTINGS_MENU, false};
			} else if (input == "q") {
				return {nullptr, false}; 
			} else {
				return {nullptr, true};
			}
		}
	};

	const ArrowMenuConfig TIMER_MENU = {
		ready::timer_list,
		[](std::string input) -> MenuResponce {
			Timer t;
			// Здесь логика валидации внутри edit_timer
			if (input == "hourly" || input == "daily") {
				t.edit_timer(input);
				return {(BaseMenu*)&SETTINGS_MENU, false};
			} else {
				return {nullptr, true};
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
				std::string keyword = MenuManager::getInstatce().request_input<std::string>("Write new keyword: ");
				if (!keyword.empty()) {
					k.Format(keyword); 
					keywords.push_back(keyword);
					Config::getInstance().set("/search/keywords", keywords);
				}
				return {nullptr, false};
			} else if (input == "r") {
				int display_index = MenuManager::getInstatce().request_input<int>("Enter index for remove: ");
				if (display_index >= 1) {
					int real_index = display_index - 1;
					if (real_index < (int)keywords.size()) {
						keywords.erase(keywords.begin() + real_index);
						Config::getInstance().set("/search/keywords", keywords);
					}
				}
				return {nullptr, false};
			} else if (input == "m") {
				k.open_keywords_editor();
				Config::getInstance().loadConfig();
				return {nullptr, false};
			} else if (input == "q") {
				return {(BaseMenu*)&MAIN_MENU, false};
			} else {
				return {nullptr, true};
			}
		}
	};

	const CharacterMenuConfig SETTINGS_MENU = {
		"12q",
		generators::settings_list,
		[](std::string input) -> MenuResponce {
			if (input == "1") {
				return {(BaseMenu*)&TIMER_MENU, false};
			} else if (input == "2") {
				return {nullptr, false}; // Место для логики пути
			} else if (input == "q") {
				return {(BaseMenu*)&MAIN_MENU, false};
			} else {
				return {nullptr, true};
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
			return {"none", "hourly", "daily"};
		}
	}

	std::string CharacterMenuConfig::get_input() const { return io::CharacterInput(*this); }
	std::string ArrowMenuConfig::get_input() const { return io::ArrowInput(*this); }

}
