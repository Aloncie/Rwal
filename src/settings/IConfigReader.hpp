#pragma once
#include "logs/logs.hpp"

#include <nlohmann/json.hpp>
#include <string>

class IConfigReader {
public:
	virtual ~IConfigReader() = default;
	IConfigReader() = default;
	template<typename G>
	G get(const std::string& key) {
		try {
			nlohmann::json j = getImpl(key);
			return j.get<G>();
		} catch (std::invalid_argument& e) {
			Logs::getInstance().writeLogs("Error of getting config data for key: " + key + ". " + std::string(e.what()));
			return G{};
		}
	}
	
	template<typename S>
	void set(const std::string& key, const S& value) {
		nlohmann::json jValue = value;
		if (!setImpl(key, jValue)) {
			Logs::getInstance().writeLogs("Failed to set config data for key: " + key);
		}
	}

	virtual void reload() = 0;
	virtual nlohmann::json& all() = 0;
	virtual nlohmann::json getImpl(const std::string& key) = 0;
	virtual bool setImpl(const std::string& key, const nlohmann::json& value) = 0;

};
