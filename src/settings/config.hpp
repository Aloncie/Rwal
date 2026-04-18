#pragma once
#include "logs/logs.hpp"
#include "IConfigReader.hpp"

#include <functional>
#include <QFileSystemWatcher>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <stdexcept>
#include <QObject>

class Config : public QObject, public IConfigReader {
    Q_OBJECT
private:
    nlohmann::json data;
    std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
    std::string configPath;

    void saveConfig();
    void initValidators();
	
public slots:
    void loadConfig();

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
            m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Config, "Validation failed for key: " + key);
            return false;
        }
        data[nlohmann::json::json_pointer(key)] = value;
        saveConfig();
        return true;
    }

public:
    QFileSystemWatcher* watcher;

    Config(Logs& logs);

    void reload() override { loadConfig(); }

    std::string getConfigPath();
    nlohmann::json& all() override { return data; }
};

