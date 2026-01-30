#pragma once
#include <functional>
#include <QFileSystemWatcher>
#include <nlohmann/json.hpp>
#include <string>
#include "logs/logs.hpp"

class Config : public QObject{
	Q_OBJECT
private:
	QFileSystemWatcher* watcher;
	nlohmann::json data;
	std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
	std::string configPath;

	void saveConfig();
	void initValidators();
	
	Config();

	Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

public slots:
	void loadConfig();
public:
	static Config& getInstance() {
        static Config instance;
        return instance;
    }

	std::string getConfigPath();
	nlohmann::json& all() { return data; }

	template<typename G>
	G get(const std::string& key){
		try {
			return data[key].get<G>();
		}catch(std::invalid_argument& e){
			 
			Logs::getInstance().write_logs("Error of getting config data: " + std::string(e.what()));
			return G{};
		}
	}

	template<typename S>
	bool set(const std::string& key, const S& value){
		nlohmann::json jValue = value;

		if (validators.count(key) && !validators.at(key)(jValue))
			return false;

		data[nlohmann::json::json_pointer(key)] = jValue;
		saveConfig();	
		return true;
	}
};
