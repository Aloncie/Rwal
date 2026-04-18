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

Config::Config(Logs& logs) : IConfigReader(logs) {
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
                m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Config, "Config loaded/reloaded: " + configPath);
            } catch (nlohmann::json::parse_error& e) {
                m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Config, "JSON Parse Error: " + std::string(e.what()));
            }
        }
    } else {	
		data = {
   			{"services", {
        		{"wallhaven", {
            		{"apikey", ""},
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
				{"random_page", true}

    		}},
			{"settings", {
				{"cursor-visibility", true}
			}}
		};
		saveConfig();
	};		

	if (watcher && watcher->files().isEmpty()) {
        watcher->addPath(QString::fromStdString(configPath));
    }
}

void Config::saveConfig(){
	if (watcher) watcher->blockSignals(true); // Stops the watcher from triggering the fileChanged signal
	try {
		std::ofstream file(configPath);
		if (file.is_open()){
			file << data.dump(4);
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Config, "Config saved: " + configPath);
		}
		else {
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Config, "Failed to open file: " + configPath);
		}
	} catch (nlohmann::json::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Config, "JSON Save Error: " + std::string(e.what()));
	}
	if (watcher) watcher->blockSignals(false); // Turns the watcher back on
}

void Config::initValidators(){
	auto is_not_empty_string = [](const nlohmann::json& j){
		return j.is_string() && !j.get<std::string>().empty(); 
				};
	auto is_not_empty_array = [](const nlohmann::json& j){
		return j.is_array() && !j.empty();
	};
	auto is_not_empty_bool = [](const nlohmann::json& j){
        return j.is_boolean();
    };
	auto is_not_empty_object = [](const nlohmann::json& j){
        return (j.is_object() || j.is_array()) && !j.empty();
    };

	validators["/search/keywords"] = is_not_empty_array;
   	validators["/services/wallhaven/apikey"] = is_not_empty_string;
	validators["/search/sorting"] = is_not_empty_string;
    validators["/search/res"] = is_not_empty_string;
    validators["/settings/cursor-visibility"] = is_not_empty_bool;
	validators["/search/random_page"] = is_not_empty_bool;
	validators["/services/wallhaven/base_url"] = is_not_empty_string;
	validators["/services/wallhaven/param_names/query"] = is_not_empty_string;
    validators["/services/wallhaven/param_names/sorting"] = is_not_empty_string;
    validators["/services/wallhaven/param_names/res"] = is_not_empty_string;
	validators["/services/wallhaven/param_names"] = is_not_empty_array;
	validators["/services/wallhaven"] = is_not_empty_object;
	validators["/search"] = is_not_empty_object;
	validators["/services"] = is_not_empty_object;
    validators["/"] = is_not_empty_object;
}


