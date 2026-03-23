#include "keywords.hpp"
#include "logs/logs.hpp"
#include "internal/platform/env_utils.hpp"
#include "funcs/funcs.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include "internal/utils/string_utils.hpp"
#include "internal/utils/vector_utils.hpp"

Keywords::Keywords(UIManager& ui, Config& config) : m_ui(ui), m_config(config){};

void Keywords::promptForKeywords(std::function<void(std::vector<std::string>)> callback) {
    m_ui.requestInput<std::string>(
        [this, callback](std::string input) {
            rwal::utils::string::format(input);
            auto keywords = rwal::utils::string::split_by_space(input);
            if (keywords.empty()) {
                m_ui.showMessage("Input cannot be empty! Try again.");
                promptForKeywords(callback);
            } else {
                m_config.set("/search/keywords", keywords);
                callback(keywords);
            }
        },
        "Keywords not found. Enter keywords (space separated): "
    );
}

void Keywords::LongWayGetKeywords(std::function<void(std::vector<std::string>)> callback){
    auto search = m_config.get<nlohmann::json>("search");

    if (search.contains("keywords") && !search["keywords"].empty()){
        callback(search["keywords"].get<std::vector<std::string>>());
		return;
    }
   	
	promptForKeywords(callback);
}

void Keywords::Default(std::vector<std::string>& keywords){
	 
	Logs::getInstance().writeLogs("There are not keywords. The default keywords will be used.");
	keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals", "anime", "cars"};
}

void Keywords::GetRandomKeywords(std::function<void(std::string)> callback, const std::string& mode){
    if (mode == "change"){
        std::vector<std::string> keywords = ShortWayGetKeywords();
        if (keywords.empty()) Default(keywords);
		else callback(keywords[random(keywords.size() - 1)]);
    }
    else if (mode == "core"){
        LongWayGetKeywords([this, callback](std::vector<std::string> keywords){
            if (keywords.empty()) Default(keywords);
			else callback(keywords[random(keywords.size() - 1)]);
        });
    }
}

void Keywords::editKeywords(){
    fs::path temp_path = fs::temp_directory_path() / "keywords.txt";    
    importToTxt(temp_path);
   	 
    def_prog_mode();
    endwin();
    
    rwal::platform::executor::open_editor(temp_path);    
    
    refresh();
    
    auto keywords = exportFromTxt(temp_path);
    m_config.set("/search/keywords", keywords);
    fs::remove(temp_path);
}

void Keywords::importToTxt(fs::path& path){

	std::ofstream file(path);
	std::vector<std::string> keywords = ShortWayGetKeywords();	
	for (int i = 0; i < keywords.size(); i++)
	   file << keywords[i] + "\n";
	file.close();

}

std::vector<std::string> Keywords::exportFromTxt(fs::path& path){
	std::ifstream file(path);
	std::string line;
	std::vector<std::string> keywords;

	if (file.is_open()){
		while (getline(file,line)){
			if (line[0] != '#'){
				rwal::utils::string::format(line);
				if (!line.empty() && line.length() < 256)
					rwal::utils::vector::add_unique(keywords,line);
			}
		}
		file.close();
	} else{
		Logs::getInstance().writeLogs("Failed opening keywords.txt in " + path.string());
		m_ui.showMessage("Failed operation. More info in logs");
	}
	return keywords;
}
