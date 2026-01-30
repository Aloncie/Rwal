#include "keywords.hpp"
#include "logs/logs.hpp"
#include "internal/platform/env_utils.hpp"
#include "funcs/funcs.hpp"
#include "ui/cli/cli.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <sys/stat.h>
#include "settings/config.hpp"

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

	Format(new_keywords);	

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

void Keywords::Format(std::string& str){
	std::replace_if(str.begin(), str.end(),
		[]( unsigned char c){ return !std::isalnum(c); }, ' ');

}

void Keywords::Default(std::vector<std::string>& keywords){
	 
	Logs::getInstance().write_logs("There are not keywords. The default keywords will be used.");
	keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals"};
}

std::string Keywords::GetRandomKeywords(const std::string& mode){
	std::vector<std::string> keywords;
	if (mode == "change"){
		keywords = ShortWayGetKeywords();
	}
	else if (mode == "core")
		keywords = LongWayGetKeywords();
	if (keywords.empty())
		Default(keywords);

	return keywords[random(keywords.size() - 1)];
		
}

void Keywords::editKeywords(){
	std::string path = std::string(SOURCE_DIR) + "keywords.txt";
	importToTxt(path);
	std::vector<std::string> keywords;
	
	rwal::platform::terminal::buffer::enter_alt_buffer();
	rwal::platform::executor::open_editor(path);	
	rwal::platform::terminal::buffer::leave_alt_buffer();
	keywords = exportFromTxt(path);

	Config::getInstance().set("/search/keywords", keywords);
}

void Keywords::importToTxt(std::string& path){

	std::ofstream file(path);
	std::vector<std::string> keywords = ShortWayGetKeywords();	
	for (int i = 0; i < keywords.size(); i++)
	   file << keywords[i];
	file.close();

}

std::vector<std::string> Keywords::exportFromTxt(std::string& path){
	std::ifstream file(path);
	std::string line;
	std::vector<std::string> keywords;

	if (file.is_open()){
		while (getline(file,line)){
			Format(line);
			if (!line.empty() && line.length() < 256)
				if (std::find(keywords.begin(), keywords.end(), line) == keywords.end())
					keywords.push_back(line);
		}
		file.close();
	} else{
		Logs::getInstance().write_logs("Failed opening keywords.txt in " + path);
		MenuManager::getInstatce().show_message("Failed operation. More info in logs");
	}
	return keywords;
}
