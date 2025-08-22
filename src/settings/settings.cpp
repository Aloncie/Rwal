#include "settings.h"
#include "logs/logs.h"
#include <filesystem>

fs::path get_pictures_path(){
	Logs l;
	l.write_logs("Try to know 'Pictures' location");

	#ifdef _WIN32
	const char* user_profile = std::getenv("USERPROFILE"); 
	if (user_profile){
		l.write_logs("Pictures location: " + std::string(user_profile) + "\\Pictures");
		return user_profile+="\\rwal\\";
	}
	#else
	const char* home = std::getenv("HOME");
	if (home){
		fs::path pictures_path = fs::path(home) / "Pictures";	
		if (fs::exists(pictures_path)){
			l.write_logs("Pictures location: " + pictures_path.string());
			return pictures_path+="/rwal/";	
		}
		pictures_path = fs::path(home) / "Изображения";
		if (fs::exists(pictures_path)){
			l.write_logs("Pictures location: " + pictures_path.string());
			return pictures_path+="/rwal/";
		}
	}
	#endif
	l.write_logs("Failed trying");
	return "None";
}

std::string rwal_catalog(){
	Logs l;
	fs::path pictures_path = get_pictures_path();
	l.write_logs("The pictures_path: " + pictures_path.string());
	try {
		if (pictures_path == "None")
			return "None";
		else if (!fs::exists(pictures_path)){
			fs::create_directory(pictures_path);
			l.write_logs("Catalog 'rwal' created\nFull path:" + std::string(pictures_path));
		}
		else 
			return pictures_path.string();

	} catch (std::exception& e){
		l.write_logs("Filesystem error in catalog creating/checking: " + std::string(e.what()));	
	}
	return "None";
}

void Timer::setup_systemd_timer(){
	Logs l;
	l.write_logs("Try to create/check service&timer files");
	const fs::path service_dir = "/etc/systemd/system";
	const std::string service_name = "rwal";
	const fs::path service_file = service_dir / (service_name + ".service");
	const fs::path timer_file = service_dir / (service_name + ".timer");

	if (getuid() != 0) {
		l.write_logs("Failed - there are not root rights");
		std::cerr << "Need root rights. Please run program with 'sudo'" << std::endl; 	
		return;
	}
	
	if (!fs::exists(service_file)){
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
			"WorkingDirectory=/home/p1rat/code/rwal\n"
			"Environment=DISPLAY=:0\n"
			"Environment=XAUTHORITY=/run/user/1000/xauth_fEDteC\n"
			"Environment=DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus\n"
			"Restart=on-failure\n"
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
	if (!fs::exists(timer_file)){
		l.write_logs("There is not timer file");
			l.write_logs("Try to create timer file");
			std::ofstream service(timer_file);
			if (service.is_open()){
				service <<
				"[Unit]\n"
				"Description=Activates rwal.service periodicallyn\n"
				"[Timer]\n"
				"OnCalendar=hourly\n"
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
	system(("systemctl enable --now " + service_name + ".timer").c_str());
}

std::string Timer::see_timer(){
	std::ifstream file("/etc/systemd/system/rwal.timer");	
	std::string str;
	Logs l;
	l.write_logs("Try to read timer file");

	if (file.is_open()){
		while (getline(std::cin,str)){
			if (str.starts_with("OnCalendar=")){
				str.erase(str.starts_with("OnCalendar="));
				l.write_logs("Successful reading. Data: " + str);
				return str;
			}
		}
	}
	else 
		l.write_logs("Failed to read timer file");

	return "None";
}

void Timer::edit_timer(){
		
}
