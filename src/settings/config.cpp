#include "config.h"
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <fstream>

std::string Config::getConfigPath(){
	QCoreApplication::setApplicationName("rwal");
	QCoreApplication::setOrganizationName("Aloncie");

	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir dir;

	if (!dir.exists(configDir))
		dir.mkpath(configDir);

	return ((configDir + "/config.json").toStdString());
}

Config::Config(){
	configPath = getConfigPath();
	initValidators();	
	if (fs::exists(configPath)){
		 
		Logs::getInstance().write_logs("Config file exists: " + configPath);
		std::ifstream file(configPath);
		data = nlohmann::json::parse(file);				
	}
	else{
		data = {
			{"search", {
				{"keywords", {}}
			}},
			{"api", {
				{"wallhaven_api_key", ""}
			}}
		};
		saveConfig();
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



