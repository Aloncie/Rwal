#include "LinuxSystemSchedule.hpp"
#include "internal/platform/env_utils.hpp"

#include <fstream>
#include <exception>
#include <string_view>

#include <unistd.h>

namespace lvl = rwal::logs::types;
namespace mod = rwal::logs::modules;

using namespace rwal::systemd;

bool LinuxSystemSchedule::create() {
	if (!m_fs.exists(m_serviceFile)){
		bool success = createService();
		if (!success){
			return false;
		}
	}
	if (!m_fs.exists(m_timerFile)){
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
		if (code == 0) {
			m_logs.writeLogs(lvl::Info, mod::Schedule, "Success status check, result is true");
			return true;
		}
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to check timer status: " + std::string(e.what()));
	}
	m_logs.writeLogs(lvl::Info, mod::Schedule, "Success status check, result is false");
	return false;
}
bool LinuxSystemSchedule::createService(){
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to create service file");
	std::string executable_path = m_fs.getBinaryLocation().string();
	std::ofstream service(m_serviceFile);
	if (service.is_open()){
		service <<
		"[Unit]\n"
		"Description=Changes wallpaper\n\n"
		"[Install]\n"
		"WantedBy=default.target\n\n"
		"[Service]\n"
		"Type=oneshot\n"
		"ExecStart=\"" + executable_path + "\" --change\n"
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
	return true;
}

bool LinuxSystemSchedule::createTimer(){
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to create timer file");
	std::ofstream timer(m_timerFile);
	if (timer.is_open()){
		timer <<
		"[Unit]\n"
		"Description=Activates " + std::string(rwal::constants::files::SERVICE_FILE) + " periodically\n\n"
		"[Timer]\n"
		"OnCalendar=\n"
		"Unit=" + std::string(rwal::constants::files::SERVICE_FILE) + "\n\n"
		"[Install]\n"
		"WantedBy=timers.target\n";
		timer.close();
		m_logs.writeLogs(lvl::Info, mod::Schedule, "Success creation timer file");
	}
	else{
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to create timer file");
		return false;		
	}
	return true;
}

bool LinuxSystemSchedule::reload() {
	try {
		rwal::systemd::exec("systemctl --user daemon-reload ");
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to reload daemon: " + std::string(e.what()));
		return false;
	}
	m_logs.writeLogs(lvl::Info, mod::Schedule, "Daemon successfully reloaded");
	return true;
}

bool LinuxSystemSchedule::start() const {
	try {
		rwal::systemd::exec("systemctl --user enable --now " + std::string(rwal::constants::files::TIMER_FILE));
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to start daemon: " + std::string(e.what()));
		return false;
	}
	m_logs.writeLogs(lvl::Info, mod::Schedule, "Timer successfully started");
	return true;
}

bool LinuxSystemSchedule::disable() const {
	try {
		rwal::systemd::exec("systemctl --user disable --now " + std::string(rwal::constants::files::TIMER_FILE) + std::string(rwal::constants::systemd::SUPPRESS_OUTPUT));
	} catch (const std::exception& e) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to disable daemon: " + std::string(e.what()));
		return false;
	}
	m_logs.writeLogs(lvl::Info, mod::Schedule, "Timer successfully disabled");
	return true;
}

std::string LinuxSystemSchedule::get() const {
	m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to check timer status");
	
	bool enabled = status();
	if (!enabled) {
		m_logs.writeLogs(lvl::Info, mod::Schedule, "Timer isn't active");
		return "None";
	}

	if (!m_fs.exists(m_timerFile)){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Timer file doesn't exist");
		return "Not found";
	}

	std::string line;
	std::ifstream file(m_timerFile);

	if (file.is_open()){
		while (getline(file,line)){
			if (line.starts_with("OnCalendar=")) {
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

	if (value == "None") {
		m_logs.writeLogs(lvl::Debug, mod::Schedule, "Try to disable timer");
		if (status()){
			bool disabled = disable();
			if (disabled) return "Successfully disabled";
			else return failedLog;
		}
		return "Already disabled";

	}

	if (!m_fs.exists(m_serviceFile)) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Service file doesn't exist");
		return failedLog;
	} else if (!m_fs.exists(m_timerFile)) {
        m_logs.writeLogs(lvl::Error, mod::Schedule, "Timer file doesn't exist");
        return failedLog;
    }

	std::ifstream in_file(m_timerFile);
	std::vector<std::string> lines;
	std::string line;
	bool found = false;
	
	if (!in_file.is_open()) {
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to open rwal.timer to read");
		return failedLog;
	}	
	while (getline(in_file,line)){
		if (!line.starts_with("OnCalendar=")){
			lines.push_back(line);
		} else {
			found = true;
			lines.push_back("OnCalendar=" + value);
		}
	}

	in_file.close();

	if (!found){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to find string 'OnCalendar' in timer file");
		return failedLog;
	}

	std::fstream out_file(m_timerFile, std::ios::out);	
	if (!out_file.is_open()){
		m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to create/open rwal.timer to write");
		return failedLog;
	}
	for (auto& l : lines) out_file << l << "\n";
	out_file.close();

	m_logs.writeLogs(lvl::Info, mod::Schedule, "Successful rewrite file");

	rwal::systemd::exec("systemctl --user unmask " + std::string(rwal::constants::files::TIMER_FILE) + std::string(rwal::constants::systemd::SUPPRESS_OUTPUT));
	reload();
	start();
	if (status()){
		return "Schedule successfuly activated!";
	}
	m_logs.writeLogs(lvl::Error, mod::Schedule, "Failed to activate schedule. Status: " + std::to_string(status()));
	return "Failed to activate schedule. More info in logs.";
}
