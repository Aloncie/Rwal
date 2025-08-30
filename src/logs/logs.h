#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Logs{
private:
	std::string get_current_time();
	void refresh_logs(fs::path& logs_path);
public:
	void write_logs(std::string message);
};
