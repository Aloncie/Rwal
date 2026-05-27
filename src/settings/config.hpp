#pragma once
#include "logs/logs.hpp"
#include "IConfigReader.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <stdexcept>

class Config : public IConfigReader {
private:
    nlohmann::json data;
    std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
    std::string configPath;

    void saveToFile();
    void initValidators();
	
    void getConfigFileData();
	std::string getConfigPath();
protected:
    nlohmann::json getImpl(const std::string& key) override {
        if (data.contains(nlohmann::json::json_pointer(key))) {
            return data[nlohmann::json::json_pointer(key)];
        } else {
            throw std::invalid_argument("Key not found: " + key);
        }
    }
    bool setImpl(const std::string& key, const nlohmann::json& value) override {
        if (validators.count(key) && !validators.at(key)(value)) {
            m_logs.writeLogs(lvl::Warning, mod::Config, "Validation failed for key: " + key);
            return false;
        }
        data[nlohmann::json::json_pointer(key)] = value;
        saveToFile();
        return true;
    }

public:
    Config(Logs& logs, IFileSystem& fs);
    void reload() override { getConfigFileData(); }

    nlohmann::json& all() override { return data; }
};

