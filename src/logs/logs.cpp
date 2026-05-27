#include "logs.hpp"
#include "internal/AppConfig.h.in"

#ifndef _WIN32
	#include <unistd.h>
	#include <sys/stat.h>
	#include <pwd.h>
#endif

#include <deque>
#include <chrono>
#include <format>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

Logs::Logs(IFileSystem& fs) : m_fs(fs) {
	logs_path = m_fs.getTempLocation() / ORGANIZATION_NAME / APPLICATION_NAME / "logs.txt";
	if (!m_fs.exists(logs_path.parent_path())) {
		m_logs.writeLogs(lvl::Info, mod::Core, "Logs directory not found, creating it");
		m_fs.createDirectories(logs_path);
	}
    if (m_fs.exists(logs_path)) {
        const uintmax_t fileSize = m_fs.getFileSize(logs_path);
        const uintmax_t limit_size = 1024 * 1024; // 1 MB

        if (fileSize > limit_size){
			writeLogs(lvl::Warning, mod::Core, "Logs file size (" + std::to_string(fileSize) + " bytes) exceeds the limit (" + std::to_string(limit_size) + " bytes). Try to refresh logs.");
			refresh();
		}
    }
    f.open(logs_path, std::ios::app);
};

std::string Logs::getCurrentTime() const {
	auto now = std::chrono::system_clock::now();
	auto local_time = std::chrono::current_zone()->to_local(now);

	return std::format("[:%Y-%m-%d | %H:%M:%S]", local_time);
}

void Logs::writeLogs(std::string_view type, std::string_view module, std::string_view message){
    if (f.is_open()) {
        f << getCurrentTime() << type << module << " " << message << "\n";
		
		if (type == lvl::Fatal){
			f.flush();
		}
    }
}

bool Logs::refresh() {
    try {
        if (fs::remove(logs_path)){
            writeLogs(lvl::Info, mod::Core, "Successful deleting old logs");
			return true;
		}
		return false;
    } catch (const std::exception& e) {
        writeLogs(lvl::Error, mod::Core, "Failed to refresh logs: " + std::string(e.what()));
    }

    std::ofstream f(logs_path, std::ios::out);
    f.close();

#ifndef _WIN32
    if (chmod(logs_path.c_str(), 0644) != 0) {
        writeLogs(lvl::Error, mod::Core, "Failed to change mod of logs\n Try to fix it yourself");
		return false;
    }
    if (geteuid() == 0) {
        const char* sudo_user = std::getenv("SUDO_USER");
        if (sudo_user) {
            struct passwd* pw = getpwnam(sudo_user);
            if (pw) {
                if (chown(logs_path.c_str(), pw->pw_uid, pw->pw_gid) != 0) {
                    writeLogs(lvl::Error, mod::Core, "Failed to change owner of logs\n Try to fix it yourself");
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

