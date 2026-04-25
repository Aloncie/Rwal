#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// ============================================================
// Logging Types & Modules
// ============================================================

namespace rwal::logs{
	namespace modules{
		inline constexpr std::string_view Network = "[NETWORK]";
		inline constexpr std::string_view UI = "[UI]";
		inline constexpr std::string_view Wallpaper = "[WALLPAPER]";
		inline constexpr std::string_view Config = "[CONFIG]";
		inline constexpr std::string_view Keywords = "[KEYWORDS]";
		inline constexpr std::string_view Filesystem = "[FILESYSTEM]";
		inline constexpr std::string_view Core = "[CORE]";
		inline constexpr std::string_view Navigator = "[NAVIGATOR]";
		inline constexpr std::string_view Scheduler = "[SCHEDULER]";
	}
	namespace types {
		inline constexpr std::string_view Debug = "[DEBUG]";
		inline constexpr std::string_view Info = "[INFO]";
		inline constexpr std::string_view Warning = "[WARNING]";
		inline constexpr std::string_view Error = "[ERROR]";
		inline constexpr std::string_view Fatal = "[FATAL]";
	}
}

// ============================================================
// Logs Class
// ============================================================

class Logs {
private:
    fs::path logs_path;
    std::ofstream f;
  
    std::string getCurrentTime() const;
public:
	virtual bool refresh();
    Logs(); 

	virtual void writeLogs(std::string_view type, std::string_view module, std::string_view message);
	virtual std::string getLogs(const int& LinesCount = 100) const;
};

