#pragma once
#include "logs/logs.hpp"

#include <nlohmann/json.hpp>
#include <string>

class IConfigReader {
protected:
	Logs& m_logs;
public:
	virtual ~IConfigReader() = default;
	explicit IConfigReader(Logs& logs) : m_logs(logs) {}
	template<typename G>
	G get(const std::string& key) {
		try {
			nlohmann::json j = getImpl(key);
			return j.get<G>();
		} catch (std::invalid_argument& e) {
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Config, "Error getting config data for key: " + key + ". " + std::string(e.what()));
			return G{};
		}
	}
	
	template<typename S>
	void set(const std::string& key, const S& value) {
		nlohmann::json jValue = value;
		if (!setImpl(key, jValue)) {
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Config, "Failed to set config data for key: " + key);
		}
	}

protected:
	virtual void reload() = 0;
	virtual nlohmann::json& all() = 0;
	virtual nlohmann::json getImpl(const std::string& key) = 0;
	virtual bool setImpl(const std::string& key, const nlohmann::json& value) = 0;

};

