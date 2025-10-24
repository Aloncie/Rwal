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
	
	if (fs::exists(configPath)){
		Logs l;
		l.write_logs("Config file exists: " + configPath);
		std::ifstream file(configPath);
		data = nlohmann::json::parse(file);				
	}
	else{
		data = {
			{"search", {
				{"keywords", {"anime", "girls"}}
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


