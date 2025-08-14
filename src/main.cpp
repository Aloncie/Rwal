#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"
#include "logs/logs.h"

int main(int argc,char **argv){
	if (true){
		Logs l;
		l.write_logs("Starting program");
	}
	int x = menu();
	while (x != 4){
		if (x == 1)
		   	refresh_wallpaper(argc,argv); 
		else if (x == 2)
			save_wallpaper(where_are_wallpaper()); 
		else if (x == 3){ 
				Keywords k;
				std::string keywords = k.look_keywords();
				if (keywords == "")
					std::cout << "The are not keywords, refresh wallpaper and write it\n";
				else 
					std::cout << "Keywords: " << keywords << std::endl;
		}
		x = menu();
		clear_last_lines(7);
	}
	Logs l;
	l.write_logs("Ending program");	
	return 0;
}
