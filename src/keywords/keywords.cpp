#include "keywords.h"
#include "logs/logs.h"
#include "CLI/CLI.h"
#include <iostream>
#include <cctype>
#include <unistd.h>
#include <sys/stat.h>
#include <settings/config.h>
#include <settings/config.h>

std::vector<std::string> Keywords::get_keywords(){
	Config c;
	auto search = c.get<nlohmann::json>("search");
	std::vector<std::string> ready_keywords = search["keywords"];
	
	if (ready_keywords.size() > 0)
		return ready_keywords;
	
	Keywords k;

	std::cout << "\nThere is not file with keywords." <<  "\nPlease write keywords to find wallpaper( use ',' to devide them): ";
	MenuManager::getInstatce().countOperatorPlus(3);
	
	std::string new_keywords = "";
	while (new_keywords == "")
		std::getline(std::cin,new_keywords);

	//Format keywords
	for (int i = 0;i < new_keywords.size(); i++){
		if (!std::isalpha(new_keywords[i])&&new_keywords[i+1] != ' '){
			new_keywords[i] = ' ';
		}
		else if (!std::isalpha(new_keywords[i])){
			new_keywords.erase(i,1);
		}
	}

	//Devide keywords
	std::string t = "";
	for (size_t i = 0; i < new_keywords.size()+1; i++) {
		if (new_keywords[i] == ' ' || i  == new_keywords.size()){
			ready_keywords.push_back(t);
			t = "";
		}
		else
			t += new_keywords[i];
	}

	
	//Save keywords
    search["keywords"] = ready_keywords;
    c.set("search", search);	

	return ready_keywords;
}

void Keywords::Default(std::vector<std::string>& keywords){
	Logs l;
	l.write_logs("There are not keywords. The default keywords will be used.");
	keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals"};
}

void Keywords::open_keywords_editor(){
	/*
    Logs l;
	struct stat buffer;
    if (stat(keywords_path.c_str(), &buffer) != 0) {
		l.write_logs("Error: File doesn't exist or inaccessible: " + keywords_path);
		MenuManager::getInstatce().show_message("Error: File doesn't exist or inaccessible: " + keywords_path);
        return;
    }

    if (access(keywords_path.c_str(), W_OK) != 0) {
		l.write_logs("Error: No write permissions for file: " + keywords_path);
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
