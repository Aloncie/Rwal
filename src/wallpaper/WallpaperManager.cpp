#include "WallpaperManager.h"
#include "/home/p1rat/code/rwal/src/keywords/keywords.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

void save_wallpaper(std::string from){
	try{
		std::string to = "/home/p1rat/Pictures" + from.substr(21,41);
		std::cout << to << std::endl;
		fs::copy_file(from,to,fs::copy_options::overwrite_existing);	
	} catch (const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
}
