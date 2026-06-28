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
    // Validate config m_data
    for (auto& [key, value] : m_data.items()) {
        auto error = m_validator.validate(key, value);
        if (error) {
            m_logs.writeLogs(
                lvl::Warning, mod::Config, "Validation failed for key: " + key + ": " + *error);
            m_refactoredData[key] = m_defaultData[key];
        }
    }
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

void Config::saveToFile() {
    // Use temporary file to avoid data loss
    auto tmp = getConfigPath().string() + ".tmp";
    std::ofstream file(tmp);
    file << m_data.dump(2);
    file.close();
    m_fs.rename(tmp, configPath); // replacement
}
