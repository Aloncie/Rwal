#pragma once
#include "IConfigReader.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"
#include "ValidatorRegistry.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

class Config : public IConfigReader {
private:
    nlohmann::json m_data;
    ValidatorRegistry m_validator;
    fs::path configPath;

    void saveToFile();

    void getConfigFileData();
    fs::path getConfigPath();
protected:
    nlohmann::json getImpl(const std::string& key) override {
        if (m_data.contains(nlohmann::json::json_pointer(key))) {
            return m_data[nlohmann::json::json_pointer(key)];
        } else {
            // throw invalid argument exception and handle in in get() (IConfigReader)
            m_logs.writeLogs(lvl::Error, mod::Config, "Key not found: " + key);
            throw std::invalid_argument("Key not found: " + key);
        }
    }
    bool setImpl(const std::string& key, const nlohmann::json& value) override {
        auto error = m_validator.validate(key, value);

        if (error) {
            m_logs.writeLogs(lvl::Warning, mod::Config, "Validation failed for key: " + key + ": " + *error);
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
