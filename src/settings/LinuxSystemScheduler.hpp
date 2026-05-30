#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "internal/AppConstants.hpp"

#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class LinuxSystemScheduler : public ISystemScheduler{
private:
	Logs& m_logs;
	IFileSystem& m_fs;
	bool createTimer();
	bool createService();
	
	const fs::path m_serviceFile;
	const fs::path m_timerFile;
public:
	LinuxSystemScheduler(Logs& logs, IFileSystem& fs) : 
		m_logs(logs), m_fs(fs),
		m_serviceFile(fs.getSchedulerLocation() / fs::path(rwal::constants::files::SERVICE_FILE)),
		m_timerFile(fs.getSchedulerLocation() / fs::path(rwal::constants::files::TIMER_FILE))
	{}
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() override;
	bool status() const override;
	bool start() const override;
	bool disable() const override;
};

