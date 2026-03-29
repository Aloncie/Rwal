#include "keywords.hpp"
#include "logs/logs.hpp"
#include "internal/platform/env_utils.hpp"
#include "funcs/funcs.hpp"
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <curses.h>
#include "internal/utils/string_utils.hpp"
#include "internal/utils/vector_utils.hpp"

Keywords::Keywords(UIManager& ui, IConfigReader& config) : m_ui(ui), m_config(config) {};

void Keywords::promptForKeywords(std::function<void(std::vector<std::string>)> callback) {
    m_ui.requestInput<std::string>(
        [this, callback](std::string input) {
            rwal::utils::string::format(input); auto keywords = rwal::utils::string::split_by_space(input);
            
            if (keywords.empty()) {
                m_ui.showMessage("Input cannot be empty! Try again.");
                promptForKeywords(callback);
            } else {
                m_config.setImpl("search", {{"keywords", keywords}}); 
                callback(keywords);
            }
        },
        "Keywords not found. Enter keywords (space separated): "
    );
}

void Keywords::LongWayGetKeywords(std::function<void(std::vector<std::string>)> callback) {
    auto search = m_config.getImpl("search");

    if (search.contains("keywords") && search["keywords"].is_array() && !search["keywords"].empty()) {
        callback(search["keywords"].get<std::vector<std::string>>());
        return;
    }
        
    promptForKeywords(callback);
}

void Keywords::Default(std::vector<std::string>& keywords) {
    Logs::getInstance().writeLogs("There are not keywords. The default keywords will be used.");
    keywords = {"nature", "landscape", "abstract", "space", "architecture", "animals", "anime", "cars"};
}

void Keywords::GetRandomKeywords(std::function<void(std::string)> callback, const std::string& mode) {
    auto selectRandom = [this, callback](std::vector<std::string>& keywords) {
        if (keywords.empty()) {
            Default(keywords);
        }

        if (keywords.empty()) {
            callback("art"); // on the 'black day'
        } else if (keywords.size() == 1) {
            callback(keywords[0]);
        } else {
            // Assumes random() is defined in funcs.hpp
            callback(keywords[random(keywords.size() - 1)]);
        }
    };

    if (mode == "change") {
        std::vector<std::string> keywords = ShortWayGetKeywords<std::vector<std::string>>();
        selectRandom(keywords);
    } 
    else if (mode == "core") {
        LongWayGetKeywords([this, selectRandom](std::vector<std::string> keywords) {
            selectRandom(keywords);
        });
    }
}

void Keywords::editKeywords() {
    fs::path temp_path = fs::temp_directory_path() / "keywords.txt";    
    importToTxt(temp_path);
        
    def_prog_mode();
    endwin();
    
    rwal::platform::executor::open_editor(temp_path);    
    
    refresh();
    
    auto keywords = exportFromTxt(temp_path);
    m_config.setImpl("search", {{"keywords", keywords}});
    fs::remove(temp_path);
}

void Keywords::importToTxt(fs::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) return;

    std::vector<std::string> keywords = ShortWayGetKeywords<std::vector<std::string>>();    
    for (const auto& kw : keywords) {
       file << kw << "\n";
    }
    file.close();
}

std::vector<std::string> Keywords::exportFromTxt(fs::path& path) {
    std::ifstream file(path);
    std::string line;
    std::vector<std::string> keywords;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            rwal::utils::string::format(line);
            if (!line.empty() && line.length() < 256) {
                rwal::utils::vector::add_unique(keywords, line);
            }
        }
        file.close();
    } else {
        Logs::getInstance().writeLogs("Failed opening keywords.txt in " + path.string());
        m_ui.showMessage("Failed operation. More info in logs");
    }
    return keywords;
}
