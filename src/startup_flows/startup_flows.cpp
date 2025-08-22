#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"
#include "logs/logs.h"
#include "startup_flows.h"
#include "settings/settings.h"
#include "CLI/Menus.h"

const MenuConfig SETTINGS_MENU = {"12q",Settings_menu}; 
const MenuConfig MAIN_MENU = {"1234q", Main_menu};
const MenuConfig KEYWORDS_MENU = {"1q", Keywords_menu};


void Flows::core(int argc,char **argv){
	if (true){
		Logs l;
		l.write_logs("Starting program");
	}

	std::map<std::string,int> countStr;
	
	countStr.insert({"menu",8});	
	countStr.insert({"keywords_menu",5});

	int count = 0;

	MenuManager m(count);
	char x = m.display(MAIN_MENU);
	while (x != 'q'){
		if (x == '1'){
		   	refresh_wallpaper(argc,argv,"core",count); 
			count += countStr.find("menu")->second;
			m.clear_last_lines();
		}
		else if (x == '2'){
			save_wallpaper(where_are_wallpaper()); 
			count += countStr.find("menu")->second;
			m.clear_last_lines();
			std::cout << "Successful download image" << std::endl;
			count++;
		}
		else if (x == '3'){ 
				Keywords k;
				std::string keywords = k.look_keywords();
				count += countStr.find("menu")->second;
				m.clear_last_lines();
				x = m.display(KEYWORDS_MENU);	
				while (x != 'q'){
					if (x == '1'){
						k.open_keywords_editor();
						count += countStr.find("keywords_menu")->second;
						m.clear_last_lines();
					}
					x = m.display(KEYWORDS_MENU);
				}
				count += countStr.find("keywords_menu")->second;
				m.clear_last_lines();
				x = 0;
		}
		else if (x == '4'){
			std::string wallpaper_local = rwal_catalog();
			Timer t;
			std::string timer = t.see_timer();
			char y  = 0;
			while (y != 'q'){
				if (y == '1'){
					
				}
				x = m.display(SETTINGS_MENU);
			}
		}
		x = m.display(MAIN_MENU);
	}
	Logs l;
	l.write_logs("Ending program");	
}

