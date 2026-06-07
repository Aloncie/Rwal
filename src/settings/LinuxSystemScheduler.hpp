#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"
#include "internal/filesystem/IFileSystem.hpp"
#include "SchedulerTypes.hpp"
#include "internal/AppConstants.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class LinuxSystemScheduler : public ISystemScheduler{
private:
	bool createTimer();
	bool createService();
	
	const fs::path m_serviceFile;
	const fs::path m_timerFile;
public:
	LinuxSystemScheduler(Logs& logs, IFileSystem& fs) : 
		ISystemScheduler(logs, fs), 
		m_serviceFile(fs.getSchedulerLocation() / fs::path(rwal::constants::files::SERVICE_FILE)),
		m_timerFile(fs.getSchedulerLocation() / fs::path(rwal::constants::files::TIMER_FILE))
	{}
	std::optional<TaskSchedulerType> get() const override;
	std::string set(TaskSchedulerType type) override;
protected:
	bool create() override;
	bool reload() override;
	bool status() const override;
	bool start() const override;
	bool disable() const override;
};

