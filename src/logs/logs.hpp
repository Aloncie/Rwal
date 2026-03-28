#pragma once
#include <string>
#include <filesystem> 
#include <fstream>
#include <QStandardPaths>
#include "ui/cli/UIManager.hpp"

namespace fs = std::filesystem;

class Logs {
private:
    fs::path logs_path;
    std::ofstream f;
    static Logs* s_instance;
    UIManager* m_ui;

    Logs() {
        logs_path =
            fs::path(
                QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString()) /
            "logs.txt";
        std::filesystem::create_directories(logs_path.parent_path());
        if (fs::exists(logs_path)) {
            const std::uintmax_t fileSize = fs::file_size(logs_path);
            const std::uintmax_t limit_size = 1024 * 1024;

            if (fileSize > limit_size) refresh(logs_path);
        }
        f.open(logs_path, std::ios::app);
    };

    Logs(const Logs&) = delete;
    Logs& operator=(const Logs&) = delete;

    std::string getCurrentTime();
    void refresh(fs::path& logs_path);

public:
    static void init(UIManager& ui) { getInstance().m_ui = &ui; }
    static Logs& getInstance() {
        if (!s_instance) {
            static Logs instance;
            s_instance = &instance;
        }
        return *s_instance;
    }

    static void setInstanceForTesting(Logs* mock) { s_instance = mock; }

    virtual void writeLogs(std::string message);
};
