#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"
#include "logs/logs.h"
#include "startup_flows.h"
#include "settings/settings.h"
#include "CLI/Menus.h"

void Flows::core(int argc,char **argv){
	if (true){
		Logs l;
		l.write_logs("Starting program");
	}
	
	int count = 0;

	MenuManager m(count);
	char x = m.charactarInput(MAIN_MENU);
	while (x != 'q'){
		if (x == '1'){
		   	refresh_wallpaper(argc,argv,"core",count); 
		}
		else if (x == '2'){
			save_wallpaper(where_are_wallpaper()); 
			std::cout << "Successful download image" << std::endl;
			m.countOperatorPlus(1);
		}
		else if (x == '3'){ 
				Keywords k;
				x = m.charactarInput(KEYWORDS_MENU);	
				while (x != 'q'){
					if (x == '1'){
						k.open_keywords_editor();
					}
					x = m.charactarInput(KEYWORDS_MENU);
				}
				x = 0;
		}
		else if (x == '4'){
			std::string wallpaper_local = rwal_catalog();
			Timer t;
			std::string timer = t.see_timer();
			x = m.charactarInput(SETTINGS_MENU);
			while (x != 'q'){
				if (x == '1'){
					m.clear_last_lines();
					std::string y = m.arrowInput(TIMER_MENU);
					std::cout << t.edit_timer(y);
				}
				x = m.charactarInput(SETTINGS_MENU);
			}
		}
		x = m.charactarInput(MAIN_MENU);
	}
	Logs l;
	l.write_logs("Ending program");	
}

