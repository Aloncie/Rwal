#include "config.hpp"

#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>

std::string Config::getConfigPath(){
	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir dir;

	if (!dir.exists(configDir))
		dir.mkpath(configDir);

	return ((configDir + "/config.json").toStdString());
}

Config::Config() {
    configPath = getConfigPath();
    initValidators();

    watcher = new QFileSystemWatcher(this);
    watcher->addPath(QString::fromStdString(configPath));
    
	loadConfig();
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &Config::loadConfig);
}

void Config::loadConfig(){
	if (std::filesystem::exists(configPath)) {
        std::ifstream file(configPath);
        if (file.is_open()) {
            try {
                data = nlohmann::json::parse(file);
                Logs::getInstance().writeLogs("Config loaded/reloaded: " + configPath);
            } catch (nlohmann::json::parse_error& e) {
                Logs::getInstance().writeLogs("JSON Parse Error: " + std::string(e.what()));
            }
        }
    } else {	
		data = {
   			{"services", {
        		{"wallhaven", {
            		{"apikey", "apikey="},
            		{"base_url", "https://wallhaven.cc/api/v1/search"},
            		{"param_names", {
                		{"query", "?q="},
                		{"sorting", "sorting"},
                		{"res", "resolutions"}
            	}}
        	}}
    		}},
    		{"search", {
        		{"keywords", {}},
        		{"sorting", "random"},
        		{"res", "1920x1080"},
				{"random_page", "true"}

    		}},
			{"settings", {
				{"cursor-visibility", "true"}
			}}
		};
		saveConfig();
	};		

	if (watcher && watcher->files().isEmpty()) {
        watcher->addPath(QString::fromStdString(configPath));
    }
}

void Config::saveConfig(){
		std::ofstream file(configPath);
		file << data.dump(4);
}

void Config::initValidators(){
	auto is_not_empty_string = [](const nlohmann::json& j){
		return j.is_string() && !j.get<std::string>().empty(); 
				};
	auto is_not_empty_array = [](const nlohmann::json& j){
		return j.is_array() && !j.empty();
	};


	validators["/timer"] = [is_not_empty_string](const nlohmann::json& j){
		if (!is_not_empty_string(j)) return false;
		char x = j.get<std::string>().back();
		return (x == 'm' || x == 'h');
	};
	validators["/search/keywords"] = is_not_empty_array;
   	validators["api/wallhaven_api_key"] = is_not_empty_string;
}


