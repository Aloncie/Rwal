#pragma once
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class Logs{
private:
	fs::path logs_path;
	std::ofstream f;

	Logs(){
		logs_path = fs::path(SOURCE_DIR) / "logs.txt";
		if (fs::exists(logs_path)){
			const std::uintmax_t fileSize = fs::file_size(logs_path);
			const std::uintmax_t limit_size = 1024 * 1024;

			if (fileSize > limit_size)
				refresh_logs(logs_path);
		}
		f.open(logs_path, std::ios::app);
	};

	Logs(const Logs&) = delete;
	Logs& operator=(const Logs&) = delete;

	std::string get_current_time();
	void refresh_logs(fs::path& logs_path);
public:

	static Logs& getInstance(){
		static Logs instance;
		return instance;
	};
	void write_logs(std::string message);
};
