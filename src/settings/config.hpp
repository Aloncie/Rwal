#pragma once
#include "logs/logs.hpp"
#include "IConfigReader.hpp"
#include "internal/filesystem/IFileSystem.hpp"

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

class Config : public IConfigReader {
private:
    nlohmann::json m_data;
    std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
	fs::path configPath;

    void saveToFile();
    void initValidators();
	
    void getConfigFileData();
	fs::path getConfigPath();

protected:
    nlohmann::json getImpl(const std::string& key) override {
        if (m_data.contains(nlohmann::json::json_pointer(key))) {
            return m_data[nlohmann::json::json_pointer(key)];
        } else {
            throw std::invalid_argument("Key not found: " + key);
        }
    }
    bool setImpl(const std::string& key, const nlohmann::json& value) override {
        if (validators.count(key) && !validators.at(key)(value)) {
            m_logs.writeLogs(lvl::Warning, mod::Config, "Validation failed for key: " + key);
            return false;
        }
        m_data[nlohmann::json::json_pointer(key)] = value;
        saveToFile();
        return true;
    }

public:
    Config(Logs& logs, IFileSystem& fs);
    void reload() override { getConfigFileData(); }

    nlohmann::json& all() override { return m_data; }
};

