#include "keywords.h"
#include "logs/logs.h"
#include <funcs/funcs.h>
#include "ui/cli/cli.h"
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

void Keywords::open_keywords_editor(){
	std::string path = std::string(SOURCE_DIR) + "keywords.txt";
   	std::ofstream file(path);
	
	std::vector<std::string> keywords = ShortWayGetKeywords();	
	for (int i = 0; i < keywords.size(); i++)
	   file << i;	
	file.close();
	
	std::cout << "\033[?1049h" << std::flush;

	const char* editor = getenv("EDITOR");
	if (!editor)
		editor = "nano";
	std::string command = std::string(editor) + " " + path;
	system(command.c_str());	

	std::cout << "\033[?1049l" << std::flush;
}
