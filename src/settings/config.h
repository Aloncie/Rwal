#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "logs/logs.h"

class Config{
private:
	nlohmann::json data;
	std::string configPath;
	std::string getConfigPath();
	void saveConfig();
public:
	Config();

	template<typename G>
	G get(const std::string& key){
		try {
			if (data.contains(key))
				return data[key].get<G>();
			else
				throw std::invalid_argument("Invalid argument - no value");
		}catch(std::invalid_argument& e){
			Logs l;
			l.write_logs("Error of getting config data: " + std::string(e.what()));
			return G{};
		}
	}

	template<typename S>
	void set(const std::string& key, const S& value){
		data[key] = value;
		saveConfig();	
	}
};
