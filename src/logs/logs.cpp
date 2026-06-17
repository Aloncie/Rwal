#include "logs.hpp"

#include "AppConfig.h"

#ifndef _WIN32
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <chrono>
#include <deque>
#include <format>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

Logs::Logs(IFileSystem& fs) : m_fs(fs) {
    logs_path = m_fs.getTempLocation() / ORGANIZATION_NAME / APP_NAME / "logs.txt";
    if (!m_fs.exists(logs_path.parent_path())) {
        writeLogs(lvl::Info, mod::Logs, "Logs directory not found, creating it");
        m_fs.createDirectories(logs_path);
    }
    if (m_fs.exists(logs_path)) {
        auto input = m_fs.getFileSize(logs_path);

        uintmax_t fileSize = 0;
        const uintmax_t limit_size = 1024 * 1024; // 1 MB

        if (input == std::nullopt) {
            writeLogs(
                lvl::Error, mod::Logs, "Failed to get logs file size: " + m_fs.getLastError());
            writeLogs(lvl::Info, mod::Logs, "Try to refresh logs to avoid possible memory filling");
            // input == std::nullopt is meaning that file doesn't exist or error occurred
            // So we can try to refresh logs to avoid possible memory filling
            refresh();
            return;
        }
        fileSize = input.value();

        if (fileSize >= limit_size) {
            writeLogs(
                lvl::Warning, mod::Logs,
                "Logs file size (" + std::to_string(fileSize) + " bytes) exceeds the limit (" +
                    std::to_string(limit_size) + " bytes). Try to refresh logs.");
            refresh();
        }
    }
    f.open(logs_path, std::ios::app);
};

std::string Logs::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();

    // Get time
    auto local_time = std::chrono::current_zone()->to_local(now);

    // Hard round to seconds
    auto local_seconds = std::chrono::time_point_cast<std::chrono::seconds>(local_time);

    // Return formatted string
    return std::format("[{:%F | %T}]", local_seconds);
}

void Logs::writeLogs(std::string_view type, std::string_view module, std::string_view message) {
    if (f.is_open()) {
        f << getCurrentTime() << type << module << " " << message << "\n";

        if (type == lvl::Fatal) {
            f.flush();
        }
    }
}

bool Logs::refresh() {
    try {
        if (fs::remove(logs_path)) {
            writeLogs(lvl::Info, mod::Logs, "Successful deleting old logs");
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        writeLogs(lvl::Error, mod::Logs, "Failed to refresh logs: " + std::string(e.what()));
    }

    std::ofstream f(logs_path, std::ios::out);
    f.close();

#ifndef _WIN32
    if (chmod(logs_path.c_str(), 0644) != 0) {
        writeLogs(lvl::Error, mod::Logs, "Failed to change mod of logs\n Try to fix it yourself");
        return false;
    }
    if (geteuid() == 0) {
        const char* sudo_user = std::getenv("SUDO_USER");
        if (sudo_user) {
            struct passwd* pw = getpwnam(sudo_user);
            if (pw) {
                if (chown(logs_path.c_str(), pw->pw_uid, pw->pw_gid) != 0) {
                    writeLogs(
                        lvl::Error, mod::Logs,
                        "Failed to change owner of logs\n Try to fix it yourself");
                    return false;
                }
            }
        }
    }
#endif
    return true;
}

std::string Logs::getLogs(const int& LinesCount) const {
    std::ifstream f(logs_path);
    if (!f.is_open()) {
        return "Failed to open logs";
    }

    // we use deque because logs.size() less 1MB, deque has no-overhead
    std::deque<std::string> lines;
    std::string line;
    while (std::getline(f, line)) {
        lines.push_back(line);
        if (lines.size() > LinesCount) {
            lines.pop_front();
        }
    }
    f.close();
    std::string result;
    for (int i = 0; i < LinesCount && !lines.empty(); ++i) {
        result += lines.front() + "\n";
        lines.pop_front();
    }
    return result;
}
