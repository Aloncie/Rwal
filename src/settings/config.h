#pragma once
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include "logs/logs.h"

class Config{
private:
	nlohmann::json data;
	std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
	std::string configPath;
	std::string getConfigPath();
	void saveConfig();
	void initValidators();
public:
	Config();

	template<typename G>
	G get(const std::string& key){
		try {
			return data[key].get<G>();
		}catch(std::invalid_argument& e){
			Logs l;
			l.write_logs("Error of getting config data: " + std::string(e.what()));
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
