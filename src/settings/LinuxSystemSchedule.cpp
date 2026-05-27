#include "LinuxSystemSchedule.hpp"
#include "internal/AppConstants.hpp"
#include "internal/platform/env_utils.hpp"

#include <fstream>
#include <exception>
#include <string_view>

// Dependence on Linux
#include <unistd.h>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

using namespace rwal::systemd;

std::optional<fs::path> LinuxSystemSchedule::getLocation() const {
	const char* home_dir = std::getenv("HOME");	
	
	if (home_dir){
		const fs::path service_dir = fs::path(home_dir) / fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER);
		try{
			fs::create_directory(service_dir);
		} catch (const std::exception& e){
			 
			m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed create service directory(" + std::string(service_dir) + "\nError: " + std::string(e.what()));
		}
		return service_dir;
	}
	else{
		m_logs.writeLogs(lvl::Error, mod::Core, "Error: HOME environment variable is not set!");	
		return std::nullopt;
	}
}

bool LinuxSystemSchedule::create() {
	const fs::path service_file = fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER) / fs::path(rwal::constants::files::SERVICE_FILE);
	const fs::path timer_file = fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER) / fs::path(rwal::constants::files::TIMER_FILE);

	// Not log because failure logging is done in createService and createTimer methods
	if (!fs::exists(service_file)){
		bool success = createService();
		if (!success){
			return false;
		}
	}
	if (!fs::exists(timer_file)){
        bool success = createTimer();
        if (!success){
            return false;
        }
    }
    return true;
}

bool LinuxSystemSchedule::status() const {
	try {
		int code = rwal::systemd::exec("systemctl --user is-active " + std::string(rwal::constants::files::TIMER_FILE));
		if (code == 0) return true;
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Core, "Failed to check timer status: " + std::string(e.what()));
	}
	return false;
}
bool LinuxSystemSchedule::createService(){
	const char* home_dir = std::getenv("HOME");
	const fs::path service_file = fs::path(home_dir) / fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER) / fs::path(rwal::constants::files::SERVICE_FILE);
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to create service file");
	std::ofstream service(service_file);
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Service file path: " + std::string(service_file));
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
		m_logs.writeLogs(lvl::Info, mod::Schedule, "Success creation service file");
	}
	else{
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to create service file");
		return false;		
	}
	return reload();
}

bool LinuxSystemSchedule::createTimer(){
	const char* home_dir = std::getenv("HOME");
	const fs::path timer_file = fs::path(home_dir) / fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER) / fs::path(rwal::constants::files::TIMER_FILE);
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to create timer file");
	std::ofstream timer(timer_file);
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Timer file path: " + std::string(timer_file));
	if (timer.is_open()){
		timer << "[Unit]\n"
		<< "Description=Activates " << rwal::constants::files::SERVICE_FILE << " periodically\n\n"
		<< "[Timer]\n"
		<< "OnCalendar=\n"
		<< "Unit=" << rwal::constants::files::SERVICE_FILE << "\n\n"
		<< "[Install]\n"
		<< "WantedBy=timers.target\n";
		timer.close();
		m_logs.writeLogs(lvl::Info, mod::Schedule, "Success creation timer file");
	}
	else{
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to create timer file");
		return false;		
	}
	return reload();
}

bool LinuxSystemSchedule::reload() {
	try {
		rwal::systemd::exec("systemctl --user daemon-reload ");
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to reload daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemSchedule::start() const {
	try {
		rwal::systemd::exec("systemctl --user enable --now " + std::string(rwal::constants::files::TIMER_FILE));
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to start daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemSchedule::disable() const {
	try {
		rwal::systemd::exec("systemctl --user disable --now " + std::string(rwal::constants::files::TIMER_FILE));
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to disable daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

std::string LinuxSystemSchedule::get() const {
	auto getLocationInput = getLocation();
	if (!getLocationInput.has_value()){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to get location");
		return "Not found";
	} 
	fs::path location = getLocationInput.value();
	std::string line;
	std::ifstream file(location / rwal::constants::files::TIMER_FILE);
	 
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to read timer file");
	
	bool enabled = status();
	if (file.is_open()){
		if (!enabled) {
			m_logs.writeLogs(lvl::Info, mod::Schedule, "Timer isn't active");
			return "None";
		}
		while (getline(file,line)){
			if (line.starts_with("OnCalendar=") != std::string::npos) {
				line.erase(0,line.find("=")+1);
				m_logs.writeLogs(lvl::Info, mod::Schedule, "Successful reading. Data: " + line);
				return line;
			}
		}
	}
	m_logs.writeLogs(lvl::Warning, mod::Schedule, "Can't read timer file");
	return "Not found";
}

std::string LinuxSystemSchedule::set(const std::string& value) {
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to edit timer");
	const std::string failedLog = "Failed set timer. More info in logs.";	
	if (!create()) return failedLog;

	auto getLocationInput = getLocation();
	if (!getLocationInput.has_value()){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to get location");
		return failedLog;
	}
	fs::path location = getLocationInput.value();
	std::ifstream in_file(location  / rwal::constants::files::TIMER_FILE);
	std::vector<std::string> lines;
	std::string line;
	bool found = false;
	
	if (!in_file.is_open()) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to open rwal.timer to read");
		return failedLog;
	}	
	while (getline(in_file,line)){
		if (line.starts_with("OnCalendar=") == std::string::npos){
			lines.push_back(line);
		} else {
			found = true;
			if (value == "None") {
				m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to disable timer");
				lines.push_back(line);
				if (!status()){
					bool disabled = disable();
					if (disabled) return "Successfully disabled";
					else return failedLog;
				}
				else return "Already disabled";
			} else {
				lines.push_back("OnCalendar=" + value);
				if (!start()) {
					m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to start timer");
					return failedLog;
				}
			}
		}
	}

	in_file.close();

	if (!found){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to find string 'OnCalendar' in timer file");
		return failedLog;
	}

	std::fstream out_file(location  / rwal::constants::files::TIMER_FILE, std::ios::out);	
	if (!out_file.is_open()){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to create/open rwal.timer to write");
		return failedLog;
	}
	for (auto& l : lines) out_file << l << "\n";
	out_file.close();

	m_logs.writeLogs(lvl::Info, mod::Schedule, "Successful rewrite file");
	if (value != "None") {
		rwal::systemd::exec("systemctl --user unmask " + std::string(rwal::constants::files::TIMER_FILE));
		reload();
		start();
		if (status()){
			m_logs.writeLogs(lvl::Info, mod::Schedule, "Schedule successfuly activated");
			return "Schedule successfuly activated!";
		}
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to activate schedule. Status: " + std::to_string(status()));
		return "Failed to activate schedule. More info in logs.";
	}
	else{
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to set timer. Value: " + value + ". Status: " + std::to_string(status()));
		return failedLog;
	}
}
