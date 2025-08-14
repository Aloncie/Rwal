#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"
#include "logs/logs.h"

int main(int argc,char **argv){
	if (true){
		Logs l;
		l.write_logs("Starting program");
	}

	int count = 0;
	int x = menu();

	while (x != 4){
		if (x == 1){
			count = 0;
		   	refresh_wallpaper(argc,argv); 
			clear_last_lines(7+count);
		}
		else if (x == 2){
			save_wallpaper(where_are_wallpaper()); 
			clear_last_lines(7+count);
			std::cout << "\n  Successful saving image" << std::endl;
		}
		else if (x == 3){ 
				Keywords k;
				std::string keywords = k.look_keywords();
				clear_last_lines(7+count);
				int y = keywords_menu();
				while (y != 2){
					if (y == 1){
						k.open_keywords_editor();
					}
					y = keywords_menu();
				}
		}
		x = menu();
		count=2;
	}
	Logs l;
	l.write_logs("Ending program");	
	return 0;
}
