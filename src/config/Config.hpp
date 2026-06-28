#pragma once
#include "IConfigReader.hpp"
#include "ValidatorRegistry.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"

#include <filesystem>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class Config : public IConfigReader {
private:
    // Default data, broken fields will be replaced via this
    nlohmann::json m_defaultData =
                       {"services",
                        {{"wallhaven",
                          {{"apikey", ""},
                           {"base_url", "https://wallhaven.cc/api/v1/search"},
                           {"param_names",
                            {{"query", "q"},
                             {"page", "page"},
                             {"apikey", "apikey"},
                             {"sorting", "sorting"},
                             {"resolutions", "resolutions"}}}}}}},
                   {"search",
                    {{"keywords", {}},
                     {"sorting", "random"},
                     {"resolutions", "1920x1080"},
                     {"random_page", true}

                    }},
    {
        "settings", {
            { "cursor-visibility", true }
        }
    }
};

// Erorrs from validation
std::vector<std::string> m_corrections;

// Data from real file
nlohmann::json m_data;

// We will not touch data from real file to save it
// But we will use refactored version of internal if something is broken
nlohmann::json m_refactoredData;

ValidatorRegistry m_validator;
fs::path configPath;

// Save original data to file on disk
// Need for write configuration updates without fixing broken parts
// We don't should directly fix config if user don't ask us to do it
// For example, if user messed up and paste keywords in param_names and we can't remove them
//
// Rwal has user-friendly position — we will take data, refactor it and use in app.
// Don't write refactored data to file on disk, fix file data if user don't ask us to do it
bool saveToFile();

void getConfigFileData();
fs::path getConfigPath();

protected:
nlohmann::json getImpl(const std::string& key) override {
    if (m_refactoredData.contains(nlohmann::json::json_pointer(key))) {
        return m_refactoredData[nlohmann::json::json_pointer(key)];
    }

    if (m_data.contains(nlohmann::json::json_pointer(key))) {
        return m_data[nlohmann::json::json_pointer(key)];
    }

    // throw invalid argument exception and handle in in get() (IConfigReader)
    m_logs.writeLogs(lvl::Error, mod::Config, "Key not found: " + key);
    throw std::invalid_argument("Key not found: " + key);
}
bool setImpl(const std::string& key, const nlohmann::json& value) override {
    auto error = m_validator.validate(key, value);

    if (error) {
        m_logs.writeLogs(
            lvl::Warning, mod::Config, "Validation failed for key: " + key + ": " + *error);
        return false;
    }

    // Update refactored data but don't save it on disk
    // if refactorerData is null, it means that original data isn't broken
    // We don't need to refactoredData
    if (!m_refactoredData.is_null()) {
        m_refactoredData[nlohmann::json::json_pointer(key)] = value;
    }

    // rewrite data on disk even if validation failed
    m_data[nlohmann::json::json_pointer(key)] = value;
    if (saveToFile())
        return true;
    return false;
}

public:
Config(Logs& logs, IFileSystem& fs);
void reload() override {
    m_corrections.clear();
    getConfigFileData();
    validateAndTakeCorrectData();
}

nlohmann::json& all() override {
    return m_data;
}
}

// Two different save methods, one for corrected data and one for original m_data
// Merge methods in one is overkill because one is private and the other is public
// I think it's better to have two different simple save methods
bool saveCorrectedConfig();

;
