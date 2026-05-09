#pragma once
#include "logs/logs.hpp"
#include "ISystemSchedule.hpp"

#include <string>
#include <filesystem>
#include <QDir>
#include <optional>

namespace fs = std::filesystem;

class LinuxSystemSchedule : public ISystemSchedule{
private:
	Logs& m_logs;

	std::optional<fs::path> getLocation() const;
	bool createTimer();
	bool createService();

public:
	explicit LinuxSystemSchedule(Logs& logs) : m_logs(logs) {}
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() override;
	bool status() const override;
	bool start() const override;
	bool disable() const override;
};

