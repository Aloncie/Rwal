#include "settings.h"
#include "logs/logs.h"
#include "CLI/CLI.h"
#include <fstream>
#include <exception>
#include <vector>
#include <unistd.h>
#include <iostream>

fs::path get_pictures_path(){
	Logs l;
	l.write_logs("Try to know 'Pictures' location");
	fs::path rwal_path;
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
	const char* starting_path = std::getenv("SUDO_USER"); 
	std::string home_path;
	if (!starting_path){
		home_path = std::getenv("HOME");
	}
	else 
		home_path = "/home/" + std::string(starting_path);

	if (!home_path.empty()){
		rwal_path = fs::path(home_path) / "Pictures";	
		if (fs::exists(rwal_path)){
			l.write_logs("Pictures location: " + rwal_path.string());
			rwal_path /= "rwal/";
		}
		else{
			rwal_path = fs::path(starting_path) / "Изображения";
			if (fs::exists(rwal_path)){
				l.write_logs("Pictures location: " + rwal_path.string());
				rwal_path /= "rwal/";
			}
			else
				return "";
		}
	}
	#endif

	try {
		if (!fs::exists(rwal_path)){
			fs::create_directory(rwal_path);
			l.write_logs("Catalog 'rwal' created\nFull path:" + std::string(rwal_path));
		}
		else{
			l.write_logs("The rwal catalog already exists.\nFull path: " + rwal_path.string());
			return rwal_path.string();
		}
	} catch (std::exception& e){
		l.write_logs("Filesystem error in catalog creating/checking: " + std::string(e.what()));
	}

	l.write_logs("Failed trying");
	return "";
}

void Timer::create_systemd_timer(){

	//we must have a root rights
	//be careful with it if u want to call this func in some space
	
	Logs l;
	l.write_logs("Try to create/check service&timer files");
	const fs::path service_dir = "/etc/systemd/system";
	const std::string service_name = "rwal";
	const fs::path service_file = service_dir / (service_name + ".service");
	const fs::path timer_file = service_dir / (service_name + ".timer");

	if (!fs::exists(service_file)||fs::file_size(service_file) == 0){
		l.write_logs("There is not service file");
		l.write_logs("Try to create service file");
		std::ofstream service(service_file);
		if (service.is_open()){
			service <<
			"[Unit]\n"
			"Description=A service to refresh ur wallpaper in some time\n\n"
			"[Service]\n"
			"Type=simple\n"
			"ExecStart=/home/p1rat/code/rwal/build/rwal --change\n"
			"User=p1rat\n"
			"WorkingDirectory=/home/p1rat/code/rwal/build\n\n"
			"Environment=XDG_RUNTIME_DIR=/run/use/1000\n\n"
			"Restart=on-failure\n"
			"RestartSec=2s\n"
			"StartLimitBurst=3\n\n"	
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

	system("systemctl daemon-reload");
}

std::string Timer::see_timer(){
	std::ifstream file("/etc/systemd/system/rwal.timer");	
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
	
	if (geteuid() != 0) {
		l.write_logs("Failed - there are not root rights");
		return "\nFailed. Need root rights. Please run program with 'sudo'";
	}

	l.write_logs("Try to edit timer");
		
	create_systemd_timer();

	std::ifstream in_file("/etc/systemd/system/rwal.timer");
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
				system("systemctl daemon-reload");
				system("systemctl disable --now rwal.timer");
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

	std::ofstream out_file("/etc/systemd/system/rwal.timer");
	if (!out_file){
		l.write_logs("Failed to create/open rwal.timer to write");
		return "Failed set timer. More info in logs.";
	}
	for (auto& l : lines)
		out_file << l << "\n";

	out_file.close();
	l.write_logs("Successful edit timer. Try to active timer");
	
	system("systemctl unmask rwal.timer >/dev/null 2>&1");
	system("systemctl daemon-reload");	
	system("systemctl start rwal.timer");
	system("systemctl enable --now rwal.timer");

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
	return system("systemctl is-active rwal.timer >/dev/null 2>&1") == 0;
}


