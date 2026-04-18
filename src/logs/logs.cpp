#include "logs.hpp"

#include <unistd.h>
#include <sys/stat.h>
#include <QDateTime>
#include <QStandardPaths>
#include <unistd.h>
#include <pwd.h>
#include <deque>

Logs::Logs() {
	logs_path = fs::path(QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toStdString()) / "logs.txt";
    std::filesystem::create_directories(logs_path.parent_path());
    if (fs::exists(logs_path)) {
        const std::uintmax_t fileSize = fs::file_size(logs_path);
        const std::uintmax_t limit_size = 1024 * 1024;

        if (fileSize > limit_size){
			writeLogs("Logs file size (" + std::to_string(fileSize) + " bytes) exceeds the limit (" + std::to_string(limit_size) + " bytes). Try to refresh logs.");
			refresh();
		}
    }
    f.open(logs_path, std::ios::app);
};

std::string Logs::getCurrentTime() const {
    QDateTime now = QDateTime::currentDateTime();
    QString formatted = now.toString("[yyyy-MM-dd | HH:mm:ss]");
    return formatted.toStdString();
}

void Logs::writeLogs(std::string_view type, std::stirng_view module, std::string_view message){
    if (f.is_open()) {
        f << getCurrentTime() << type << module << " " << message << std::endl;
    }
}

bool Logs::refresh() {
    try {
        if (fs::remove(logs_path)){
            writeLogs("Successful deleting old logs");
			return true;
		}
		return false;
    } catch (const std::exception& e) {
        writeLogs("Failed to refresh logs: " + std::string(e.what()));
    }

    std::ofstream f(logs_path, std::ios::out);
    f.close();

    if (chmod(logs_path.c_str(), 0644) != 0) {
        writeLogs("Failed to change mod of logs\n Try to fix it yourself");
		return false;
    }
    if (geteuid() == 0) {
        const char* sudo_user = std::getenv("SUDO_USER");
        if (sudo_user) {
            struct passwd* pw = getpwnam(sudo_user);
            if (pw) {
                if (chown(logs_path.c_str(), pw->pw_uid, pw->pw_gid) != 0) {
                    writeLogs("Failed to change owner of logs\n Try to fix it yourself");
					return false;
                }
            }
        }
    }
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

