#include "config.h"
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <fstream>

fs::path Config::getConfigPath(){
	QCoreApplication::setApplicationName("rwal");
	QCoreApplication::setOrganizationName("Aloncie");

	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir dir;

	if (!dir.exists(configDir))
		dir.mkpath(configDir);

	return ((configDir + "/config.json").toUtf8()).constData();
}

Config::Config(){
	configPath = getConfigPath();
	
	if (fs::exists(configPath)){
		data = nlohmann::json::parse(configPath);				
	}
	else{
		data = {
			{"search", {
				{"keywords", {"nature", "anime", "space"}}
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

template<typename G>
G Config::get(const std::string& key){
	return data.value(key,G{});	
}

template<typename S>
void Config::set(const std::string& key, const S& value){
	data[key] = value;
	saveConfig();	
}
