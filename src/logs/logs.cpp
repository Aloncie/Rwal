#include "logs.h"
#include <memory>
#include <chrono>
#include <iomanip>

std::string Logs::get_current_time(){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d | %H:%M:%S]");
	return ss.str();
}

void Logs::write_logs(std::string message){
	std::unique_ptr<std::ofstream> f(new std::ofstream);
	f->open("/home/p1rat/code/rwal/logs.txt", std::ios::app);
	if (f->is_open()){
		*f << get_current_time() << " " << message << "\n";
	}
	else
		std::cerr << "Error create/open logs" << std::endl;
}
