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
    QFileSystemWatcher* watcher;
    nlohmann::json data;
    std::map<std::string, std::function<bool(const nlohmann::json&)>> validators;
    std::string configPath;

    void saveConfig();
    void initValidators();
	
public slots:
    void loadConfig();

public:
    Config(Logs& logs);

    void reload() override { loadConfig(); }

    std::string getConfigPath();
    nlohmann::json& all() override { return data; }
    nlohmann::json getImpl(const std::string& key) override {
        if (data.contains(nlohmann::json::json_pointer(key))) {
            return data[nlohmann::json::json_pointer(key)];
        } else {
            throw std::invalid_argument("Key not found: " + key);
        }
    }
    bool setImpl(const std::string& key, const nlohmann::json& value) override {
        if (validators.count(key) && !validators.at(key)(value)) {
            m_logs.writeLogs("Validation failed for key: " + key);
            return false;
        }
        data[nlohmann::json::json_pointer(key)] = value;
        saveConfig();
        return true;
    }

    template <typename G>
    G get(const std::string& key) {
        try {
            nlohmann::json j = getImpl(key);
            return j.get<G>();
        } catch (std::invalid_argument& e) {
            m_logs.writeLogs("Error of getting config data for key: " + key + ". " + std::string(e.what()));
            return G{};
        }
    }

    template <typename S>
    bool set(const std::string& key, const S& value) {
        nlohmann::json jValue = value;

        if (validators.count(key) && !validators.at(key)(jValue)) {
            m_logs.writeLogs("Validation failed for key: " + key);
            return false;
        }
        data[nlohmann::json::json_pointer(key)] = jValue;
        saveConfig();
        return true;
    }
};

