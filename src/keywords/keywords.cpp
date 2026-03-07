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

std::vector<std::string> Keywords::LongWayGetKeywords(){
    auto search = m_config.get<nlohmann::json>("search");

    if (search.contains("keywords") && !search["keywords"].empty()){
        return search["keywords"].get<std::vector<std::string>>();
    }
    
    std::string input;
    std::vector<std::string> ready_keywords;

    while (ready_keywords.empty()) {
		input = m_ui.requestInput<std::string>("\nKeywords not found. Enter keywords (space separated): ");
        
        rwal::utils::string::format(input); 
        ready_keywords = rwal::utils::string::split_by_space(input);

        if (ready_keywords.empty()) {
             m_ui.showMessage("Input cannot be empty! Try again.");
        }
    }
    
    m_config.set("/search/keywords", ready_keywords); 
    return ready_keywords;
}

void Keywords::Default(std::vector<std::string>& keywords){
	 
	Logs::getInstance().write_logs("There are not keywords. The default keywords will be used.");
	keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals", "anime", "cars"};
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
	fs::path temp_path = fs::temp_directory_path() / "keywords.txt";	
	importToTxt(temp_path);
	std::vector<std::string> keywords;
	
	rwal::platform::terminal::buffer::enter_alt_buffer();
	rwal::platform::executor::open_editor(temp_path);	
	rwal::platform::terminal::buffer::leave_alt_buffer();
	keywords = exportFromTxt(temp_path);

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
		Logs::getInstance().write_logs("Failed opening keywords.txt in " + path.string());
		m_ui.showMessage("Failed operation. More info in logs");
	}
	return keywords;
}
