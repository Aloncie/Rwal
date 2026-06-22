#pragma once
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

class IConfigReader {
protected:
    Logs& m_logs;
    IFileSystem& m_fs;

public:
    virtual ~IConfigReader() = default;
    IConfigReader(Logs& logs, IFileSystem& fs) : m_logs(logs), m_fs(fs) {}
    template <typename G> G get(const std::string& key) {
        try {
            nlohmann::json j = getImpl(key);
            return j.get<G>();
        } catch (std::invalid_argument& e) {
            m_logs.writeLogs(
                lvl::Error, mod::Config,
                "Error getting config data for key: " + key + ". " + std::string(e.what()));
            return G{};
        }
    }

    template <typename S> bool set(const std::string& key, const S& value) {
        nlohmann::json jValue = value;
        if (!setImpl(key, jValue)) {
            m_logs.writeLogs(lvl::Error, mod::Config, "Failed to set config data for key: " + key);
            return false;
        }
        return true;
    }

    virtual nlohmann::json& all() = 0;
    virtual void reload() = 0;

protected:
    virtual nlohmann::json getImpl(const std::string& key) = 0;
    virtual bool setImpl(const std::string& key, const nlohmann::json& value) = 0;
};
