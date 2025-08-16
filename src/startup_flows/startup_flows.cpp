#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"
#include "logs/logs.h"

void core(int argc,char **argv){
	if (true){
		Logs l;
		l.write_logs("Starting program");
	}

	std::map<std::string,int> countStr;
	
	countStr.insert({"menu",6});	
	countStr.insert({"keywords_menu",5});

	int count = 0;
	char x = 0;
	menu(x,count);

	while (x != 'q'){
		if (x == '1'){
		   	refresh_wallpaper(argc,argv); 
			count += countStr.find("menu")->second;
			clear_last_lines(count);		
		}
		else if (x == '2'){
			save_wallpaper(where_are_wallpaper()); 
			count += countStr.find("menu")->second;
			clear_last_lines(count);
			std::cout << "Successful download image" << std::endl;
			count++;
		}
		else if (x == '3'){ 
				Keywords k;
				std::string keywords = k.look_keywords();
				count += countStr.find("menu")->second;
				clear_last_lines(count);
				char y = 0;
				keywords_menu(y,count);
				while (y != 'q'){
					if (y == '1'){
						k.open_keywords_editor();
						count += countStr.find("keywords_menu")->second;
						clear_last_lines(count);
					}
					keywords_menu(y,count);
				}
				count += countStr.find("keywords_menu")->second;
				clear_last_lines(count);
				x = 0;
		}
		menu(x,count);
	}
	Logs l;
	l.write_logs("Ending program");	
}
