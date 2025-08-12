#include "wallpaper/WallpaperManager.h"
#include "CLI/CLI.h"
#include "keywords/keywords.h"

int main(int argc,char **argv){
	Menu m;
	int x = m.menu();
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
	}
		
	return 0;
}
