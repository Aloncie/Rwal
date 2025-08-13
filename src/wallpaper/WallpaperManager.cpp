#include "WallpaperManager.h"
#include "/home/p1rat/code/rwal/src/wallpaper/WallpaperManager.h"
#include "/home/p1rat/code/rwal/src/net/CurlWrapper.h"
#include "/home/p1rat/code/rwal/src/keywords/keywords.h"
#include "/home/p1rat/code/rwal/src/dbus/PlasmaDBus.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

void save_wallpaper(std::string from){
	try{
		std::string to = "/home/p1rat/Pictures/rwall/" + from.substr(21,41);
		std::cout << to << std::endl;
		fs::copy_file(from,to,fs::copy_options::overwrite_existing);	
	} catch (const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
}

void refresh_wallpaper(int argc, char *argv[]){
	std::string url;
	std::string local;
	Keywords k;
	auto keywords = k.divide_keywords(k.get_keywords());

	do {
		std::string kw = k.choose_keyword(keywords);
		MyCurl c("an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a", kw);
		c.get_request();
		url = c.get_data("data","path");
		if (!url.empty()) {
			local = c.download_image(url);
			break;
		}
	} while (true);	

	change_wallpaper(argc, argv, local);

}

std::string where_are_wallpaper(){
	fs::path dir_path("/home/p1rat/code/rwal");
	
	try {
		for (const auto& item : fs::directory_iterator(dir_path)){
			if (fs::is_regular_file(item.path())){
				auto name = item.path().filename().string();
				if (name.rfind("wallpaper-", 0) == 0) {
					return item.path();
				}
			}
		}

	} catch (const fs::filesystem_error& e){
		Logs l;
		l.write_logs("Error of delete old image: " + std::string(e.what()));
		return nullptr;
	}
}
