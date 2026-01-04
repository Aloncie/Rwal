#include "settings/settings.h"
#include "keywords/keywords.h"
#include <nlohmann/json.hpp>
#include "settings/config.h"
#include "wallpaper/WallpaperManager.h"
#include "menus.h"
#include "ui/cli//cli.h"
#include <iostream>

using namespace rwal::ui;


const CharacterMenuConfig MAIN_MENU = {
	"1234q",                     
	ready::main_list,  
	[](std::string input) -> BaseMenu* { 
		if (input == "1") { refresh_wallpaper(); return nullptr; }
		if (input == "2") { save_wallpaper(where_are_wallpaper()); return nullptr; }
		if (input == "3") return const_cast<CharacterMenuConfig*>(&KEYWORDS_MENU);
		if (input == "4") return const_cast<CharacterMenuConfig*>(&SETTINGS_MENU);
		return nullptr;
    }
};

const ArrowMenuConfig TIMER_MENU = {
	ready::timer_list,
	[](std::string input) -> BaseMenu* {
		Timer t;
		t.edit_timer(input);
		return const_cast<CharacterMenuConfig*>(&SETTINGS_MENU);
	}
};

const CharacterMenuConfig KEYWORDS_MENU = {
    "armq", 
	generators::keywords_list,
	[](std::string input) -> BaseMenu* {
		Keywords k;
		std::vector<std::string> keywords = k.ShortWayGetKeywords();

		if (input == "a"){ 
			std::string keyword; 
			MenuManager::getInstatce().show_message("Write new keyword: ");
			std::getline(std::cin,keyword); 
			if (!keyword.empty()){
				k.Format(keyword); 
				keywords.push_back(keyword);
				Config::getInstance().set("/search/keywords", keywords);
			}
			return nullptr;
		}
		if (input == "r"){
			MenuManager::getInstatce().show_message("Enter index to remove: ");
			int display_index;
			if (std::cin >> display_index){
				int real_index = display_index - 1;
				if (real_index >= 0 && real_index < keywords.size()){
					keywords.erase(keywords.begin() + real_index);
					Config::getInstance().set("/search/keywords", keywords);

				}
				else 
					MenuManager::getInstatce().show_message("Incorrect index");
			}
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			return nullptr;

		}
		if (input == "m"){
			k.open_keywords_editor();
			Config::getInstance();	
		}


	}
};

const CharacterMenuConfig SETTINGS_MENU = {
    "12q",
   	generators::settings_list 
};

namespace generators {
	std::vector<std::string> keywords_list() {
		std::vector<std::string> lines = {"--- Keywords Editor ---"};

		Keywords k;
		std::vector<std::string> keywords = k.ShortWayGetKeywords();

		if (keywords.empty())
			lines.push_back("None");	

		else {
			for(size_t i = 0; i < keywords.size(); ++i) {
				lines.push_back(std::to_string(i + 1) + ". " + keywords[i]);
			}
		}
	
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

std::string CharacterMenuConfig::get_input() const {
    return io::CharacterInput(*this);
}

std::string ArrowMenuConfig::get_input() const {
    return io::ArrowInput(*this); 
}
