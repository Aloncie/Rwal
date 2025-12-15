#include "settings.h"
#include "logs/logs.h"
#include <filesystem>
#include <fstream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <exception>
#include <optional>
#include <vector>
#include <unistd.h>
#include <iostream>

std::optional<fs::path> Timer::get_user_timer_path() const {
	const char* home_dir = std::getenv("HOME");	
	
	if (home_dir){
		const fs::path service_dir = fs::path(home_dir) / ".config" / "systemd" / "user";
		try{
			fs::create_directory(service_dir);
		} catch (const std::exception& e){
			Logs l;
			l.write_logs("Failed create service directory(" + std::string(service_dir) + "\nError: " + std::string(e.what()));
		}
		return service_dir;
	}
	else{
		Logs l;
		l.write_logs("Error: HOME environment variable is not set!");	
		return std::nullopt;
	}
}

fs::path PicturesPath::get_pictures_path(){
	Logs l;
	l.write_logs("Try to know 'Pictures' location");
	#ifdef _WIN32
	const char* user_profile = std::getenv("USERPROFILE"); 
	if (user_profile){
		rwal_path = user_profile / "Pictures";
		if (!fs::exists(rwal_path)){
			rwal_path = user_profile / "Изображения";	
			if (!fs::exists(rwal_path)
				return "";
		}
		l.write_logs("Pictures location: " + rwal_path);
		rwal_path /= "rwal\\";
	}
	#else
	fs::path path = std::getenv("HOME");

	if (!path.empty()){
		path /= "Pictures";	
		if (fs::exists(path)){
			l.write_logs("Pictures location: " + path.string());
			path /= "rwal/";
		}
		else{
			path /= "Изображения";
			if (fs::exists(path)){
				l.write_logs("Pictures location: " + path.string());
				path /= "rwal/";
			}
			else
				return "None";
		}
	}
	#endif

	try {
		if (!fs::exists(path)){
			fs::create_directory(path);
			l.write_logs("Catalog 'rwal' created\nFull path:" + std::string(path));
		}
		else{
			l.write_logs("The rwal catalog already exists.\nFull path: " + path.string());
			return path.string();
		}
	} catch (std::exception& e){
		l.write_logs("Filesystem error in catalog creating/checking: " + std::string(e.what()));
	}

	l.write_logs("Failed getting path of rwal catalog in pictures.");
	return "None";
}

void Timer::create_systemd_timer(){
	
	Logs l;
	l.write_logs("Try to create/check service&timer files");

	auto path = get_user_timer_path();
	if (!path)
		return;

	const fs::path service_dir = *path;
	const std::string service_name = "rwal";
	const fs::path service_file = service_dir / (service_name + ".service");
	const fs::path timer_file = service_dir / (service_name + ".timer");

	if (!fs::exists(service_file)||fs::file_size(service_file) == 0){
		l.write_logs("There is not service file");
		l.write_logs("Try to create service file");
		std::ofstream service(service_file);
		if (service.is_open()){
			service <<
			"[Service]\n"
			"Type=simple\n"
			"ExecStart=/usr/local/bin/rwal --change\n"
			"Restart=on-failure\n"
			"RestartSec=2s\n"
			"StartLimitBurst=3\n"
			"[Install]\n"
			"WantedBy=default.target\n";
			service.close();
			l.write_logs("Success creation service file");
		}
		else{
			l.write_logs("Failed to create service file");
			return;		
		}
	} else
		l.write_logs("Service file already exists");
	if (!fs::exists(timer_file)||fs::file_size(timer_file) == 0){
		l.write_logs("There is not timer file");
			l.write_logs("Try to create timer file");
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
				l.write_logs("Success creation timer file");
			}
			else{
				l.write_logs("Failed to create timer file");
				return;		
			}
	} else
		l.write_logs("Timer file already exists");

	system("systemctl --user daemon-reload");
}

std::string Timer::see_timer(){
	auto path = get_user_timer_path();
	if (!path)
		return "None";

	std::ifstream file(*path / "rwal.timer");
	std::string str;
	Logs l;
	l.write_logs("Try to read timer file");

	if (file.is_open()){

		if (!check_timer_active_status())
			return "None";

		while (getline(file,str)){
			if (str.starts_with("OnCalendar=")){
				str.erase(0,str.find("=")+1);
				l.write_logs("Successful reading. Data: " + str);
				return str;
			}
		}
	}
	l.write_logs("No data");
	return "None";
}

std::string Timer::edit_timer(std::string value){
	Logs l;
	l.write_logs("Try to edit timer");
		
	create_systemd_timer();

	auto path = get_user_timer_path();
	if (!path)
		return "None";

	std::ifstream in_file(*path / "rwal.timer");
	std::vector<std::string> lines;
	std::string line;
	bool found = false;

	if (!in_file){
		l.write_logs("Failed to open rwal.timer to read");
		return "Failed set timer. More info in logs.";
	}	
	while (getline(in_file,line)){
		if (line.find("OnCalendar=") == std::string::npos)
			lines.push_back(line);
		else {
			found = true;
			if (value == "None"){
				l.write_logs("value is 'None'\nTry to disactivate timer");
				lines.push_back("OnCalendar=");
				system("systemctl unmask rwal.timer >/dev/null 2>&1");
				system("systemctl --user daemon-reload");
				system("systemctl --user disable --now rwal.timer");
				l.write_logs("Timer successfuly disactivated");
				return "Timer successfuly disactivated!";
			}
			else
				lines.push_back("OnCalendar=" + value);
		}
	}

	in_file.close();

	if (!found){
		l.write_logs("Failed to find string");
		return "Failed set timer. More info in logs.";
	}

	std::fstream out_file(*path / "rwal.timer");	
	if (!out_file){
		l.write_logs("Failed to create/open rwal.timer to write");
		return "Failed set timer. More info in logs.";
	}
	for (auto& l : lines)
		out_file << l << "\n";

	out_file.close();
	l.write_logs("Successful edit timer. Try to active timer");
	
	system("systemctl unmask rwal.timer >/dev/null 2>&1");
	system("systemctl --user daemon-reload");	
	system("systemctl --user start rwal.timer");
	system("systemctl --user enable --now rwal.timer");

	if (check_timer_active_status()){
		l.write_logs("Timer successfuly activated");
		return "Timer successfuly activated!";
	}
	else{
		l.write_logs("Failed to activate timer");
		return "Failed set timer. More info in logs.";
	}
}

bool Timer::check_timer_active_status(){
	return system("systemctl --user is-active rwal.timer >/dev/null 2>&1") == 0;
}

