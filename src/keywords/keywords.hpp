#pragma once

#include "internal/filesystem/IFileSystem.hpp"
#include "logs/logs.hpp"
#include "settings/IConfigReader.hpp"
#include "ui/IUserInterface.hpp"

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Keywords {
private:
    IConfigReader& m_config;
    Logs& m_logs;
    IFileSystem& m_fs;

protected:
    virtual void promptForKeywords(
        std::function<void(std::vector<std::string>)> callback, IUserInterface& ui,
        int attempts = 0);
    virtual std::string pickRandomKeyword(const std::vector<std::string>& keywords) const;

public:
    Keywords(IConfigReader& config, Logs& logs, IFileSystem& fs)
        : m_config(config), m_logs(logs), m_fs(fs) {}
    [[nodiscard]] virtual std::vector<std::string> loadKeywordsFromConfig() const;
    [[nodiscard]] virtual std::string SilentGetKeyword();
    virtual void Default(std::vector<std::string>& keywords) const;
    virtual void editKeywords(IUserInterface& ui);
    [[nodiscard]] virtual std::string InteractiveGetKeyword(IUserInterface& ui);
    virtual ~Keywords() = default;
};
