#include "LinuxSystemScheduler.hpp"
#include "internal/AppConstants.hpp"
#include "internal/platform/env_utils.hpp"

#include <fstream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <exception>
#include <string_view>

// Dependence on Linux
#include <unistd.h>

using namespace rwal::systemd;

std::optional<fs::path> LinuxSystemScheduler::getLocation() const {
	const char* home_dir = std::getenv("HOME");	
	
	if (home_dir){
		const fs::path service_dir = fs::path(home_dir) / fs::path(rwal::constants::dirs::LINUX_SYSTEMD_USER.data());
		try{
			fs::create_directory(service_dir);
		} catch (const std::exception& e){
			 
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed create service directory(" + std::string(service_dir) + "\nError: " + std::string(e.what()));
		}
		return service_dir;
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Core, "Error: HOME environment variable is not set!");	
		return std::nullopt;
	}
}

std::optional<bool> LinuxSystemScheduler::status() const {
	try {
		int code = exec("systemctl --user is-active " + std::string(rwal::constants::files::TIMER_FILE));
		return code == 0;
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Core, "Failed to check timer status: " + std::string(e.what()));
		return std::nullopt;
	}
}
bool LinuxSystemScheduler::createService(){
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Scheduler, "Try to create service file");
	std::ofstream service(std::string(rwal::constants::dirs::LINUX_SYSTEMD_USER.data()) + std::string(rwal::constants::files::SERVICE_FILE.data()));
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
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Success creation service file");
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to create service file");
		return false;		
	}
	return reload();
}

bool LinuxSystemScheduler::createTimer(){
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Scheduler, "Try to create timer file");
	std::ofstream service(std::string(rwal::constants::dirs::LINUX_SYSTEMD_USER.data()) + std::string(rwal::constants::files::TIMER_FILE.data()));
	if (service.is_open()){
		service << "[Unit]\n"
		<< "Description=Activates " << rwal::constants::files::SERVICE_FILE << " periodically\n\n"
		<< "[Timer]\n"
		<< "OnCalendar=\n"
		<< "Unit=" << rwal::constants::files::SERVICE_FILE << "\n\n"
		<< "[Install]\n"
		<< "WantedBy=timers.target\n";
		service.close();
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Success creation timer file");
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to create timer file");
		return false;		
	}
	return reload();
}

bool LinuxSystemScheduler::reload() const {
	try {
		exec("systemctl --user daemon-reload ");
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to reload daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemScheduler::start() const {
	try {
		exec("systemctl --user enable --now " + std::string(rwal::constants::files::TIMER_FILE));
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to start daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}

bool LinuxSystemScheduler::disable() const {
	try {
		exec("systemctl --user disable --now " + std::string(rwal::constants::files::TIMER_FILE));
	} catch (const std::exception& e) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to disable daemon: " + std::string(e.what()));
		return false;
	}
	return true;
}
std::string LinuxSystemScheduler::get() const {
	if (location.empty()) location = getLocation();
	if (!location) return "None";

	std::ifstream file(*location / rwal::constants::files::TIMER_FILE);
	std::string str;
	 
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Scheduler, "Try to read timer file");

	if (file.is_open()){
		if (status().has_value()) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "(result)Timer status: " + std::to_string(status().value()));
			if (status().value() == 0) return "None";
		}
		else {
			return "Unknown";
		}

		while (getline(file,str)){
			if (str.starts_with("OnCalendar=")){
				str.erase(0,str.find("=")+1);
				m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Successful reading. Data: " + str);
				return str;
			}
		}
	}
	m_logs.writeLogs(rwal::logs::types::Warning, rwal::logs::modules::Scheduler, "Can't read timer file");
	return "None";
}

std::string LinuxSystemScheduler::set(const std::string& value) {
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Scheduler, "Try to edit timer");
	const std::string failedLog = "Failed set timer. More info in logs.";	
	if (!create()) return failedLog;

	auto location = getLocation();
	if (!location) return failedLog;

	std::ifstream in_file(*location / rwal::constants::files::TIMER_FILE);
	std::vector<std::string> lines;
	std::string line;
	bool found = false;

	if (!in_file){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to open rwal.timer to read");
		return failedLog;
	}	
	while (getline(in_file,line)){
		if (line.find("OnCalendar=") == std::string::npos)
			lines.push_back(line);
		else {
			found = true;
			if (value == "None") {
				m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Scheduler, "Try to disable timer");
				lines.push_back(line);
				if (lines.back().find("None") == std::string::npos) {
					bool disabled = disable();
					if (disabled) return "Successfully disabled";
					else return failedLog;
				}
				else return "Already disabled";
			} else {
				lines.push_back("OnCalendar=" + value);
				if (!start()) {
					m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to start timer");
					return failedLog;
				}
			}
		}
	}

	in_file.close();

	if (!found){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to find string 'OnCalendar' in timer file");
		return failedLog;
	}

	std::fstream out_file(*location / rwal::constants::files::TIMER_FILE.data(), std::ios::out);	
	if (!out_file){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to create/open rwal.timer to write");
		return failedLog;
	}
	for (auto& l : lines) out_file << l << "\n";
	out_file.close();

	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Successful rewrite file");
	if (value != "None") {
		exec("systemctl --user unmask " + std::string(rwal::constants::files::TIMER_FILE));
		reload();
		if (status().has_value() && status().value() == 1) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Scheduler successfuly activated");
			return "Scheduler successfuly activated!";
		}
	}
	else{
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to set timer. Value: " + value + ". Status: " + std::to_string(status().value()));
		return failedLog;
	}
}
