#include "logs.h"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <fstream>

std::string Logs::get_current_time(){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d | %H:%M:%S]");
	return ss.str();
}

void Logs::write_logs(std::string message){ 
	fs::path logs_path = fs::path(SOURCE_DIR) / "logs.txt";
	if (fs::exists(logs_path)){
		const std::uintmax_t fileSize = fs::file_size(logs_path);
		const std::uintmax_t limit_size = 1024;

		if (fileSize > limit_size)
			refresh_logs(logs_path);
	}
	std::ofstream f(logs_path, std::ios::app);
	if (f.is_open()){
		f << get_current_time() << " " << message << "\n";
	}
	else
		std::cerr << "Error create/open logs" << std::endl;
}

void Logs::refresh_logs(fs::path& logs_path){
	try {
		if (fs::remove(logs_path))
			write_logs("Successful deleting old logs");
		else 
			write_logs("Failed deleting old logs");
	} catch (const std::exception& e){
		write_logs("Failed to refresh logs: " + std::string(e.what()));
	}	
	
	std::ofstream f(logs_path, std::ios::out);	
}
