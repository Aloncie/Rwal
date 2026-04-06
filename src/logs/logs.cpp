#include "logs.hpp"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

Logs::Logs() {
	logs_path = fs::path(QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString()) / "logs.txt";
    std::filesystem::create_directories(logs_path.parent_path());
    if (fs::exists(logs_path)) {
        const std::uintmax_t fileSize = fs::file_size(logs_path);
        const std::uintmax_t limit_size = 1024 * 1024;

        if (fileSize > limit_size) refresh(logs_path);
    }
    f.open(logs_path, std::ios::app);
};

std::string Logs::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d | %H:%M:%S]");
    return ss.str();
}

void Logs::writeLogs(std::string_view message) {
    if (f.is_open()) {
        f << getCurrentTime() << " " << message << std::endl;
    }
}

void Logs::refresh(fs::path& logs_path) {
    try {
        if (fs::remove(logs_path))
            writeLogs("Successful deleting old logs");
        else
            writeLogs("Failed deleting old logs");
    } catch (const std::exception& e) {
        writeLogs("Failed to refresh logs: " + std::string(e.what()));
    }

    std::ofstream f(logs_path, std::ios::out);
    f.close();

    if (chmod(logs_path.c_str(), 0644) != 0) {
        writeLogs("Failed to change mod of logs\n Try to fix it yourself");
    }
    if (geteuid() == 0) {
        const char* sudo_user = std::getenv("SUDO_USER");
        if (sudo_user) {
            struct passwd* pw = getpwnam(sudo_user);
            if (pw) {
                if (chown(logs_path.c_str(), pw->pw_uid, pw->pw_gid) != 0) {
                    writeLogs("Failed to change owner of logs\n Try to fix it yourself");
                }
            }
        }
    }
}

