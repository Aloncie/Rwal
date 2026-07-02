#include "Config.hpp"

#include "AppConfig.h"

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

fs::path Config::getConfigPath() {
    configPath = m_fs.getConfigLocation() / ORGANIZATION_NAME / APP_NAME / "config.json";

    if (!m_fs.exists(configPath.parent_path())) {
        m_logs.writeLogs(lvl::Info, mod::Config, "Config directory not found, creating it");
        m_fs.createDirectories(configPath.parent_path());
    }

    return configPath;
}

Config::Config(Logs& logs, IFileSystem& fs) : IConfigReader(logs, fs) {
    configPath = getConfigPath();

    // Init validators
    using namespace rwal::config::validators;

    // --- User settings (warnings) ---
    m_validator.add("/search/keywords", {isArray, nonEmptyArray});
    m_validator.add("/search/sorting", {isString, nonEmptyString, isValidSorting});
    m_validator.add("/search/resolutions", {isString, nonEmptyString, isValidResolution});
    m_validator.add("/settings/cursor-visibility", {isBool});
    m_validator.add("/search/random_page", {isBool});

    // --- Wallhaven service: critical infrastructure (hard checks) ---
    m_validator.add(
        "/services/wallhaven/base_url",
        {isString, isValidUrl, isExactString("https://wallhaven.cc/api/v1/search")});
    m_validator.add("/services/wallhaven/apikey", {isString}); // may be empty
    m_validator.add("/services/wallhaven/param_names", {isObject, isValidWallhavenParamNames});

    // Parents objects (just type check)
    m_validator.add("/search", {isObject});
    m_validator.add("/services", {isObject});
    m_validator.add("/services/wallhaven", {isObject});
    m_validator.add("/", {isObject});

    // Load config data
    getConfigFileData();

    m_logs.writeLogs(
        lvl::Info, mod::Config,
        "Validating config data on startup. Broken config will be replaced with default");

    validateAndTakeCorrectData();
}

void Config::getConfigFileData() {
    if (m_fs.exists(configPath)) {
        std::ifstream file(configPath);
        if (file.is_open()) {
            try {
                m_data = nlohmann::json::parse(file);
                m_logs.writeLogs(
                    lvl::Info, mod::Config, "Config loaded/reloaded: " + configPath.string());
            } catch (nlohmann::json::parse_error& e) {
                m_logs.writeLogs(
                    lvl::Error, mod::Config, "JSON Parse Error: " + std::string(e.what()));
                m_data = m_defaultData;
            }
        }
    } else {
        m_data = m_defaultData;
        saveToFile();
    };
}

bool Config::saveToFile() {
    // Use temporary file to avoid data loss
    try {
        auto tmp = getConfigPath().string() + ".tmp";
        std::ofstream file(tmp);
        file << m_data.dump(2);
        file.close();
        m_fs.rename(tmp, configPath); // replacement
        return true;
    } catch (const std::exception& e) {
        m_logs.writeLogs(lvl::Error, mod::Config, "Error saving config: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveCorrectedConfig() {
    // Use temporary file to avoid data loss
    try {
        auto tmp = getConfigPath().string() + ".tmp";
        std::ofstream file(tmp);
        file << m_refactoredData.dump(2);
        file.close();
        m_fs.rename(tmp, configPath); // replacement
        return true;
    } catch (const std::exception& e) {
        m_logs.writeLogs(
            lvl::Error, mod::Config, "Error saving corrected config: " + std::string(e.what()));
        return false;
    }
}

void Config::validateAndTakeCorrectData() {
    m_refactoredData = m_data;
    // Validate config m_data, guarantees use valid data in app
    // if validation failed, replace app version data with default but don't touch file on disk
    for (const auto& path : m_validator.paths()) {
        auto jp = nlohmann::json::json_pointer(path);
        if (m_refactoredData.contains(jp)) {
            auto error = m_validator.validate(path, m_data[jp]);
            if (error) {
                m_refactoredData[jp] = m_defaultData[jp]; // replace with default
                m_corrections.push_back(path + ": " + *error + " (reset to default)");
            }
        } else if (m_defaultData.contains(jp)) {
            m_refactoredData[jp] = m_defaultData[jp]; // missing key, add from defaults
            m_corrections.push_back(path + ": missing, added default");
        }
    }
    if (!m_corrections.empty()) {
        m_logs.writeLogs(
            lvl::Warning, mod::Config,
            "Config had " + std::to_string(m_corrections.size()) +
                " errors; using corrected values.");
        // Don't save file automatically
    } else {
        m_refactoredData = nlohmann::json(); // reset refactored data
    }
}
