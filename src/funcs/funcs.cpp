#include "funcs.h"
#include "logs/logs.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

int random(int count){
	/*
	Logs l;
	while (count > 100){
		count = count / 2;
		l.write_logs("Count = " + std::to_string(count));
	}
	*/
	std::random_device rd;
	std::mt19937 gen(rd());	
	std::uniform_int_distribution<> distrib(0,count/10);
	int num = distrib(gen);
	return num;
}

fs::path get_pictures_path(){
	Logs l;
	l.write_logs("Try to know 'Pictures' location");

	#ifdef _WIN32
	const char* user_profile = std::getenv("USERPROFILE"); 
	if (user_profile){
		l.write_logs("Pictures location: " + std::string(user_profile) + "\\Pictures);
		return user_profile+="\\rwal\\";
	}
	#else
	const char* home = std::getenv("HOME");
	if (home){
		fs::path pictures_path = fs::path(home) / "Pictures";	
		if (fs::exists(pictures_path)){
			l.write_logs("Pictures location: " + pictures_path.string());
			return pictures_path+="/rwal/";	
		}
		pictures_path = fs::path(home) / "Изображения";
		if (fs::exists(pictures_path)){
			l.write_logs("Pictures location: " + pictures_path.string());
			return pictures_path+="/rwal/";
		}
	}
	#endif
	l.write_logs("Failed trying");
	return "";
}

std::string rwal_catalog(){
	Logs l;
	fs::path pictures_path = get_pictures_path();
	l.write_logs("The pictures_path: " + pictures_path.string());
	try {
		if (!fs::exists(pictures_path)){
			fs::create_directory(pictures_path);
			l.write_logs("Catalog 'rwal' created\nFull path:" + std::string(pictures_path));
		}
		else 
			return pictures_path.string();

	} catch (std::exception& e){
		l.write_logs("Filesystem error in catalog creating/checking: " + std::string(e.what()));	
	}
}
