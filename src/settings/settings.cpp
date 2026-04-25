#include "settings.hpp"
#include "logs/logs.hpp"
#include "internal/AppConstants.hpp"

#include <fstream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <exception>
#include <string_view>

// Dependence on Linux
#include <unistd.h>

std::optional<fs::path> LinuxSystemScheduler::getLocation() const {
	const char* home_dir = std::getenv("HOME");	
	
	if (home_dir){
		const fs::path service_dir = fs::path(home_dir) / fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER.data());
		try{
			fs::create_directory(service_dir);
		} catch (const std::exception& e){
			 
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed create service directory(" + std::string(service_dir) + "\nError: " + std::string(e.what()));
		}
		return service_dir;
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Core, "Error: HOME environment variable is not set!");	
		return std::nullopt;
	}
}

bool LinuxSystemScheduler::status() const {
	try {
		bool status = system(("systemctl --user is-active " + std::string(rwal::constants::files::TIMER_FILE) + " >/dev/null 2>&1").c_str()) == 0;
		if (status) return true;
		return false;
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Core, "Failed to check timer status: " + std::string(e.what()));
		return false;
	}
}
bool LinuxSystemScheduler::create(){
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Try to create/check service&timer files");

	auto path = getLocation();
	if (!path){
		return false;
	}

	const fs::path service_dir = *path;
	const std::string service_name = rwal::constants::files::SERVICE_FILE.data();
	const fs::path service_file = rwal::constants::files::SERVICE_FILE.data();
	const fs::path timer_file = service_dir / rwal::constants::files::TIMER_FILE.data();

	if (!fs::exists(service_file)||fs::file_size(service_file) == 0){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "There is not service file");
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Try to create service file");
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
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Success creation service file");
		}
		else{
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to create service file");
			return false;		
		}
	} else
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Service file already exists");
	if (!fs::exists(timer_file)||fs::file_size(timer_file) == 0){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "There is not timer file");
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Try to create timer file");
			std::ofstream service(timer_file);
			if (service.is_open()){
				service << "[Unit]\n"
				<< "Description=Activates " << rwal::constants::files::SERVICE_FILE << " periodically\n\n"
				<< "[Timer]\n"
				<< "OnCalendar=\n"
				<< "Unit=" << rwal::constants::files::SERVICE_FILE << "\n\n"
				<< "[Install]\n"
				<< "WantedBy=timers.target\n";
				service.close();
				m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Success creation timer file");
			}
			else{
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to create timer file");
				return false;		
			}
	} else
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Scheduler file already exists");
	return reload();
}

bool LinuxSystemScheduler::reload() const {
	try {
		system("systemctl --user daemon-reload");	
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to reload daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemScheduler::start() const {
	try {
		system("systemctl --user start " + rwal::constants::files::SERVICE_FILE);
		system("systemctl --user enable --now " + rwal::constants::files::SERVICE_FILE);
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to start daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemScheduler::disable() const {
	try {
		system("systemctl --user disable " + rwal::constants::files::SERVICE_FILE);
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to disable daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}
std::string LinuxSystemScheduler::get() const {
	auto path = getLocation();
	if (!path) return "None";

	std::ifstream file(*path / rwal::constants::files::TIMER_FILE);
	std::string str;
	 
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Try to read timer file");

	if (file.is_open()){
		if (!status()) return "None";

		while (getline(file,str)){
			if (str.starts_with("OnCalendar=")){
				str.erase(0,str.find("=")+1);
				m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Successful reading. Data: " + str);
				return str;
			}
		}
	}
	m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Filesystem, "No data");
	return "None";
}

std::string LinuxSystemScheduler::set(const std::string& value) {
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Try to edit timer");
	const std::string failedLog = "Failed set timer. More info in logs.";	
	if (!create()) return failedLog;

	auto path = getLocation();
	if (!path) return "None";

	std::ifstream in_file(*path / rwal::constants::files::TIMER_FILE);
	std::vector<std::string> lines;
	std::string line;
	bool found = false;

	if (!in_file){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to open rwal.timer to read");
		return failedLog;
	}	
	while (getline(in_file,line)){
		if (line.find("OnCalendar=") == std::string::npos)
			lines.push_back(line);
		else {
			found = true;
			if (value == "None"){
				m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "value is 'None'\nTry to disactivate timer");
				lines.push_back("OnCalendar=");
				system(("systemctl unmask " + std::string(rwal::constants::files::TIMER_FILE) + " >/dev/null 2>&1").c_str());
				reload();
				disable();
				m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Scheduler successfuly disactivated");
				return "Scheduler successfuly disactivated!";
			}
			else{
				lines.push_back("OnCalendar=" + value);
			}
		}
	}

	in_file.close();

	if (!found){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to find string");
		return failedLog;
	}

	std::fstream out_file(*path / rwal::constants::files::TIMER_FILE.data(), std::ios::out);	
	if (!out_file){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to create/open rwal.timer to write");
		return failedLog;
	}
	for (auto& l : lines){
		out_file << l << "\n";
	}

	out_file.close();
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Successful edit timer. Try to active timer");
	
	system(("systemctl unmask " + std::string(rwal::constants::files::TIMER_FILE) + " >/dev/null 2>&1").c_str());

	if (status()){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Filesystem, "Scheduler successfuly activated");
		return "Scheduler successfuly activated!";
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Filesystem, "Failed to activate timer");
		return failedLog;
	}
}

