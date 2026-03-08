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

Logs* Logs::s_instance = nullptr;

std::string Logs::getCurrentTime(){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d | %H:%M:%S]");
	return ss.str();
}

void Logs::writeLogs(std::string message){ 
	if (f.is_open()){
		f << getCurrentTime() << " " << message << std::endl;
	}
	else{
		if (m_ui){
			m_ui->showMessage("Error of opening logs");
			m_ui->dodgeMessage("Error of opening logs");
		}
	}
}

void Logs::refresh(fs::path& logs_path){
	try {
		if (fs::remove(logs_path))
			writeLogs("Successful deleting old logs");
		else 
			writeLogs("Failed deleting old logs");
	} catch (const std::exception& e){
		writeLogs("Failed to refresh logs: " + std::string(e.what()));
	}	
	
	std::ofstream f(logs_path, std::ios::out);	
	f.close();
	
	if (chmod(logs_path.c_str(), 0644) != 0){
		writeLogs("Failed to change mod of logs\n Try to fix it yourself");
		if (m_ui)
			m_ui->showMessage("Critical error of logs. More info in logs.");	
	}
	if (geteuid() == 0){
		const char* sudo_user = std::getenv("SUDO_USER");
		if (sudo_user){
			struct passwd *pw = getpwnam(sudo_user);
			if (pw){
				if (chown(logs_path.c_str(), pw->pw_uid, pw->pw_gid) != 0){
					writeLogs("Failed to change owner of logs\n Try to fix it yourself");
					if (m_ui)
						m_ui->showMessage("Critical error of logs. More info in logs.");
				}
			}
		}	
	}	
}
