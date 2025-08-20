#include "Menus.h"
#include "keywords/keywords.h"
#include "settings/settings.h"

const std::vector<std::string> Timer_menu{
	"None",
	"hourly",
	"daily"
};

const std::vector<std::string> Main_manu{
	"-------------------------- ",
	"1) Refresh wallpaper now",
	"2) Save current wallpaper",
	"3) See keywords",
	"4) Settings",
	"q) Quit",
	"-------------------------- "
};

Keywords k;
const std::vector<std::string> Keywords_menu{
	"-------------------------- ",
	"Keywords: " + k.look_keywords(),
	"1) Edit keywords",
	"q) Quit",
	"-------------------------- "
};

Timer t;
std::string timer =	t.see_timer();
std::string wallpaper_local = rwal_catalog();

const std::vector<std::string> Settings_menu{
	"-------------------------- ",
	"Choose to edit any setting",
	"1) Timer: " + timer,
	"2) Wallpaper location: " + wallpaper_local,
	"q) Quit",
	"-------------------------- "
};
