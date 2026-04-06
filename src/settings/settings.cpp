#include "settings.hpp"
#include "logs/logs.hpp"

#include <filesystem>
#include <fstream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <exception>
#include <optional>
#include <unistd.h>

std::optional<fs::path> Timer::getUserTimerPath() const {
	const char* home_dir = std::getenv("HOME");	
	
	if (home_dir){
		const fs::path service_dir = fs::path(home_dir) / ".config" / "systemd" / "user";
		try{
			fs::create_directory(service_dir);
		} catch (const std::exception& e){
			 
			m_logs.writeLogs("Failed create service directory(" + std::string(service_dir) + "\nError: " + std::string(e.what()));
		}
		return service_dir;
	}
	else{
		 
		m_logs.writeLogs("Error: HOME environment variable is not set!");	
		return std::nullopt;
	}
}

void Timer::createSystemdTimer(){
	
	 
	m_logs.writeLogs("Try to create/check service&timer files");

	auto path = getUserTimerPath();
	if (!path){
		return;
	}

	const fs::path service_dir = *path;
	const std::string service_name = "rwal";
	const fs::path service_file = service_dir / (service_name + ".service");
	const fs::path timer_file = service_dir / (service_name + ".timer");

	if (!fs::exists(service_file)||fs::file_size(service_file) == 0){
		m_logs.writeLogs("There is not service file");
		m_logs.writeLogs("Try to create service file");
		std::ofstream service(service_file);
		if (service.is_open()){
			service <<
			"[Unit]\n"
			"Description=Changes wallpaper\n\n"
			"[Install]\n"
			"WantedBy=default.target\n\n"
			"[Service]\n"
			"Type=oneshot\n"
			"ExecStart=/usr/local/bin/rwal --change\n"
			"Restart=on-failure\n"
			"RestartSec=2s\n"
			"StartLimitBurst=3\n";
			service.close();
			m_logs.writeLogs("Success creation service file");
		}
		else{
			m_logs.writeLogs("Failed to create service file");
			return;		
		}
	} else
		m_logs.writeLogs("Service file already exists");
	if (!fs::exists(timer_file)||fs::file_size(timer_file) == 0){
		m_logs.writeLogs("There is not timer file");
			m_logs.writeLogs("Try to create timer file");
			std::ofstream service(timer_file);
			if (service.is_open()){
				service <<
				"[Unit]\n"
				"Description=Activates rwal.service periodically\n\n"
				"[Timer]\n"
				"OnCalendar=\n"
				"Unit=rwal.service\n\n"
				"[Install]\n"
				"WantedBy=timers.target\n";
				service.close();
				m_logs.writeLogs("Success creation timer file");
			}
			else{
				m_logs.writeLogs("Failed to create timer file");
				return;		
			}
	} else
		m_logs.writeLogs("Timer file already exists");

	system("systemctl --user daemon-reload");
}

std::string Timer::seeTimer(){
	auto path = getUserTimerPath();
	if (!path)
		return "None";

	std::ifstream file(*path / "rwal.timer");
	std::string str;
	 
	m_logs.writeLogs("Try to read timer file");

	if (file.is_open()){

		if (!TimerStatus())
			return "None";

		while (getline(file,str)){
			if (str.starts_with("OnCalendar=")){
				str.erase(0,str.find("=")+1);
				m_logs.writeLogs("Successful reading. Data: " + str);
				return str;
			}
		}
	}
	m_logs.writeLogs("No data");
	return "None";
}

std::string Timer::editTimer(std::string value){
	 
	m_logs.writeLogs("Try to edit timer");
		
	createSystemdTimer();

	auto path = getUserTimerPath();
	if (!path)
		return "None";

	std::ifstream in_file(*path / "rwal.timer");
	std::vector<std::string> lines;
	std::string line;
	bool found = false;

	if (!in_file){
		m_logs.writeLogs("Failed to open rwal.timer to read");
		return "Failed set timer. More info in logs.";
	}	
	while (getline(in_file,line)){
		if (line.find("OnCalendar=") == std::string::npos)
			lines.push_back(line);
		else {
			found = true;
			if (value == "None"){
				m_logs.writeLogs("value is 'None'\nTry to disactivate timer");
				lines.push_back("OnCalendar=");
				system("systemctl unmask rwal.timer >/dev/null 2>&1");
				system("systemctl --user daemon-reload");
				system("systemctl --user disable --now rwal.timer");
				m_logs.writeLogs("Timer successfuly disactivated");
				return "Timer successfuly disactivated!";
			}
			else
				lines.push_back("OnCalendar=" + value);
		}
	}

	in_file.close();

	if (!found){
		m_logs.writeLogs("Failed to find string");
		return "Failed set timer. More info in logs.";
	}

	std::fstream out_file(*path / "rwal.timer");	
	if (!out_file){
		m_logs.writeLogs("Failed to create/open rwal.timer to write");
		return "Failed set timer. More info in logs.";
	}
	for (auto& l : lines)
		out_file << l << "\n";

	out_file.close();
	m_logs.writeLogs("Successful edit timer. Try to active timer");
	
	system("systemctl unmask rwal.timer >/dev/null 2>&1");
	system("systemctl --user daemon-reload");	
	system("systemctl --user start rwal.timer");
	system("systemctl --user enable --now rwal.timer");

	if (TimerStatus()){
		m_logs.writeLogs("Timer successfuly activated");
		return "Timer successfuly activated!";
	}
	else{
		m_logs.writeLogs("Failed to activate timer");
		return "Failed set timer. More info in logs.";
	}
}

bool Timer::TimerStatus(){
	return system("systemctl --user is-active rwal.timer >/dev/null 2>&1") == 0;
}


