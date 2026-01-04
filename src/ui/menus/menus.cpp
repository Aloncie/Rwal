#include "keywords/keywords.h"
#include "settings/settings.h"
#include <nlohmann/json.hpp>
#include "settings/config.h"
#include "wallpaper/WallpaperManager.h"
#include "menus.h"

const CharacterMenuConfig MAIN_MENU = {
    "1234q",
	rwal::ui::ready::main_list,
	{
        {'1', []() { refresh_wallpaper(); }},
        {'2', []() { save_wallpaper(where_are_wallpaper()); }},
        {'3', []() {  }},
        {'4', []() { /* переход в settings */ }}
    }
};

const ArrowMenuConfig TIMER_MENU = {
	rwal::ui::ready::timer_list
};

const CharacterMenuConfig KEYWORDS_MENU = {
    "armq", 
	rwal::ui::generators::keywords_list
};

const CharacterMenuConfig SETTINGS_MENU = {
    "12q",
   	rwal::ui::generators::settings_list 
};

namespace rwal::ui {
	namespace generators {
		std::vector<std::string> keywords_list() {
			std::vector<std::string> lines = {"--- Keywords Editor ---"};

			auto search = Config::getInstance().get<nlohmann::json>("search");

			std::vector<std::string> keywords;
			if (search.contains("keywords"))
				if (!search["keywords"].empty()){
					keywords = search["keywords"];

					for(size_t i = 0; i < keywords.size(); ++i) {
						lines.push_back(std::to_string(i + 1) + ". " + keywords[i]);
					}
				}

				else 
					lines.push_back("None");	
		
			lines.push_back("a) Add | r) Remove | m) Manual(JSON) | q) Back");
			return lines;
		}
		std::vector<std::string> settings_list() {
			Timer t;
			PicturesPath p;

			return {
				"-------------------------- ",
				"Choose to edit any setting",
				"1) Timer: " + t.see_timer(),
				"2) Wallpaper location: " + p.get_pictures_path().string(),
				"q) Quit",
				"-------------------------- "
			};
		}
	}
	namespace ready {

		const std::vector<std::string> Main_menu{
			"-------------------------- ",
			"1) Refresh wallpaper now",
			"2) Save current wallpaper",
			"3) See keywords",
			"4) Settings",
			"q) Quit",
			"-------------------------- "
		};

		const std::vector<std::string> Timer_menu{
			"None",
			"hourly",
			"daily"
		};

	}
}

std::string CharacterMenuConfig::get_input() const {
    return rwal::ui::io::CharacterInput(*this);
}

std::string ArrowMenuConfig::get_input() const {
    return rwal::ui::io::ArrowInput(*this); 
}
