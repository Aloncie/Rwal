#include "keywords.h"
#include "logs/logs.h"
#include <funcs/funcs.h>
#include "CLI/CLI.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <sys/stat.h>
#include <settings/config.h>
#include <settings/config.h>

std::vector<std::string> Keywords::LongWayGetKeywords(){
	auto search = Config::getInstance().get<nlohmann::json>("search");

	std::vector<std::string> ready_keywords;
	if (search.contains("keywords") && !search["keywords"].empty()){
		ready_keywords = search["keywords"];
		return ready_keywords;
	}
	
	Keywords k;

	std::cout << "\nThere is not file with keywords." <<  "\nPlease write keywords to find wallpaper( use ',' to devide them): ";
	MenuManager::getInstatce().countOperatorPlus(3);
	
	std::string new_keywords = "";
	while (new_keywords == "")
		std::getline(std::cin,new_keywords);

	//Format keywords
	
	std::replace_if(new_keywords.begin(), new_keywords.end(),
			[]( unsigned char c){ return !std::isalpha(c); }, ' ');

	//Devide keywords
	
	std::stringstream ss(new_keywords);
	std::string segment;
			
	while (ss >> segment){
		ready_keywords.push_back(segment);
	}

	//Save keywords
	Config::getInstance().set("/search/keywords", ready_keywords);	

	return ready_keywords;
}

void Keywords::Default(std::vector<std::string>& keywords){
	 
	Logs::getInstance().write_logs("There are not keywords. The default keywords will be used.");
	keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals"};
}

std::string Keywords::GetRandomKeywords(const std::string& mode){
	std::vector<std::string> keywords;
	if (mode == "change"){
		keywords = ShortWayGetKeywords();
		if (keywords.size() < 1)
			Default(keywords);
	}

	else if (mode == "core")
		keywords = LongWayGetKeywords();

	return keywords[random(keywords.size() - 1)];
		
}

void Keywords::open_keywords_editor(){
	/*
     
	struct stat buffer;
    if (stat(keywords_path.c_str(), &buffer) != 0) {
		Logs::getInstance().write_logs("Error: File doesn't exist or inaccessible: " + keywords_path);
		MenuManager::getInstatce().show_message("Error: File doesn't exist or inaccessible: " + keywords_path);
        return;
    }

    if (access(keywords_path.c_str(), W_OK) != 0) {
		Logs::getInstance().write_logs("Error: No write permissions for file: " + keywords_path);
		MenuManager::getInstatce().show_message("Error: No write permissions for file: " + keywords_path);
        return;
    }

	const char* editor = getenv("EDITOR");
	if (!editor)
		editor = "nano";
	std::string command = std::string(editor) + " " + keywords_path;
	system(command.c_str());	
	*/
}
