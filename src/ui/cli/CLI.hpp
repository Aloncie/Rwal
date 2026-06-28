#pragma once
#include "config/Config.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "logs/Logs.hpp"
#include "net/NetworkManager.hpp"
#include "settings/ISystemScheduler.hpp"

#include <optional>
#include <string>

class CLI {
private:
    struct Options {
        bool showHelp = false;
        bool showVersion = false;
        bool changeWallpaper = false;
        bool saveWallpaper = false;
        bool showContact = false;
        bool showLogs = false;
        bool clearLogs = false;
        bool showKeywords = false;
        bool showScheduler = false;
        bool fixConfig = false;
        bool showConfigStatus = false;
        bool editConfig = false;

        std::optional<std::string> setKeywords;
        std::optional<std::string> addKeywords;
        std::optional<std::string> removeKeywords;
        std::optional<std::string> setScheduler;
    };

    Options m_opts;

    struct Dependencies {
        IFileSystem& fs;
        Config& config;
        NetworkManager& netmanager;
        Logs& logs;
        ISystemScheduler& scheduler;
    };

    Dependencies m_deps;

    // Handlers
    int handleHelp();
    int handleVersion();
    int handleContact();
    int handleLogs();
    int handleChange();
    int handleSave();
    int handleClearLogs();
    int handleKeywords();
    int handleSetKeywords();
    int handleRemoveKeywords();
    int handleAddKeywords();
    int handleSetScheduler();
    int handleScheduler();
    int handleFixConfig();
    int handleShowConfigStatus();
    int handleEditConfig();

    // Helper functions
    std::string getBinaryLocation();

public:
    CLI(IFileSystem& fs, Config& config, NetworkManager& nm, Logs& logs,
        ISystemScheduler& scheduler)
        : m_deps{fs, config, nm, logs, scheduler} {}
    ~CLI() = default;
    void parse(int argc, char* argv[]);
    int execute();
};
