#include "WallpaperManager.h"
#include "net/CurlWrapper.h"
#include "keywords/keywords.h"
#include "dbus/PlasmaDBus.h"
#include "funcs/funcs.h"
#include "logs/logs.h"
#include "settings/settings.h"
#include "CLI/CLI.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

void save_wallpaper(std::string from){
	Logs l;
	if (from == ""){
		MenuManager::getInstatce().show_message("Failed to save wallpaper. More info in logs");
		l.write_logs("Failed to save wallpaper. Wrong input data");
		return;
	}
	try{
		std::string rwal = get_pictures_path();
		l.write_logs("Output of rwal_catalog: " + rwal);
		std::string to = rwal + from.substr(from.find("wallpaper"));
		l.write_logs("Image successful saved: " + to);
		fs::copy_file(from,to,fs::copy_options::overwrite_existing);	
	} catch (const std::exception& e){
		l.write_logs("Failed to saved image: " + std::string(e.what()));
	}
}

void refresh_wallpaper(int argc, char *argv[],std::string mode){
	Logs l;
	std::string url, local, page, pageCount;
	int last_page = 1;
	Keywords k;
	std::vector<std::string> keywords;
	if (mode == "change")
		keywords = k.divide_keywords(k.look_keywords());
	else if (mode == "core")
		keywords = k.divide_keywords(k.get_keywords());

	do {
		std::string kw = keywords[random(keywords.size()-1)];
		MyCurl c("apikey=an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a", kw);
		c.get_request();
		pageCount = c.get_data("meta","last_page");

		try {
			last_page = std::stoi(pageCount);
		} catch(std::exception& e){
			Logs l;
			l.write_logs("Failed to stoi pageCount: " + std::string(e.what()));
			last_page = 1;
		}

		page = std::to_string(random(last_page)+1);

		MyCurl v("page=" + page + "&apikey=an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a",kw);
		v.get_request();
		url = v.get_data("data","path");
		if (!url.empty()) {
			local = v.download_image(url);
			break;
		}
	} while (true);	

	change_wallpaper(argc, argv, local);

}

std::string where_are_wallpaper(){
	fs::path dir_path =	SOURCE_DIR;
	if (dir_path == "")
		return "";
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
		l.write_logs("Error of find downloaded image: " + std::string(e.what()));
	}
	return "";
}
