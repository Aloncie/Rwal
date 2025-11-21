#include "keywords/keywords.h"
#include "settings/settings.h"
#include "Menus.h"

//local
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

//global
const MenuConfig MAIN_MENU = {
    "1234q",
    []() { return Main_menu; }
};

const MenuConfig TIMER_MENU = {
	"None",
	[]() { return Timer_menu;}
};

const MenuConfig KEYWORDS_MENU = {
    "1q", 
    []() {
        Keywords k;
		std::string keywords = k.look_keywords<std::string>();
        return std::vector<std::string>{
            "-------------------------- ",
            "Keywords: " + keywords,	
            "1) Edit keywords",
            "q) Quit",
            "-------------------------- "
        };
    }
};

const MenuConfig SETTINGS_MENU = {
    "12q",
    []() {
        Timer t;
        std::string timer = t.see_timer();
        std::string wallpaper_local = get_pictures_path();
        return std::vector<std::string>{
            "-------------------------- ",
            "Choose to edit any setting",
            "1) Timer: " + timer,
            "2) Wallpaper location: " + wallpaper_local,
            "q) Quit",
            "-------------------------- "
        };
    }
};
