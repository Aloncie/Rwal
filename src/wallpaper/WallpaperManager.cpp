#include "WallpaperManager.h"
#include "internal/GlobalConstans.h"
#include "settings/config.h"
#include "net/CurlWrapper.h"
#include "keywords/keywords.h"
#include "dbus/PlasmaDBus.h"
#include "funcs/funcs.h"
#include "logs/logs.h"
#include "settings/settings.h"
#include "net/NetworkManager.h"
#include "CLI/CLI.h"
#include <exception>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>

namespace fs = std::filesystem;

void save_wallpaper(std::string from){
	 
	if (from == ""){
		MenuManager::getInstatce().show_message("Failed to save wallpaper. More info in logs");
		Logs::getInstance().write_logs("Failed to save wallpaper. Wrong input data");
		return;
	}
	try{
		PicturesPath p;

		fs::path rwal = p.get_pictures_path();
		if (rwal == "None"){ std::cerr << "Failed operation. More in logs"; return;
		}
		Logs::getInstance().write_logs("Output of rwal_catalog: " + rwal.string());
		fs::path to = rwal / from.substr(from.find("wallpaper"));
		Logs::getInstance().write_logs("Image successful saved: " + std::string(to));
		fs::copy_file(from,to,fs::copy_options::overwrite_existing);	
	} catch (const std::exception& e){
		Logs::getInstance().write_logs("Failed to saved image: " + std::string(e.what()));
	}
}

void refresh_wallpaper(const std::string& mode){
	Keywords k;
	std::string keyword = k.GetRandomKeywords(mode);
	std::string url = NetworkManager::getInstance().fetchImage(keyword);

	change_wallpaper(url);
}

std::string where_are_wallpaper(){
	fs::path dir_path =	SOURCE_DIR / fs::path(rwal::wallpaper::DONWLOADS_DIR_NAME);
	if (dir_path == "")
		return "";
	try {
		for (const auto& item : fs::directory_iterator(dir_path)){
			if (fs::is_regular_file(item.path())){
				auto name = item.path().filename().string();
				if (name.rfind(rwal::wallpaper::FILE_PREFIX, 0) == 0) {
					return item.path();
				}
			}
		}

	} catch (const fs::filesystem_error& e){
		 
		Logs::getInstance().write_logs("Error of find downloaded image: " + std::string(e.what()));
	}
	return "";
}
