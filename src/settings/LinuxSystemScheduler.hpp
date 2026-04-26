#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"

#include <string>
#include <filesystem>
#include <QDir>
#include <optional>

namespace fs = std::filesystem;

class LinuxSystemScheduler : public ISystemScheduler{
private:
	fs::path location;
	Logs& m_logs;

	std::optional<fs::path> getLocation() const;
	bool createTimer();
	bool createService();

public:
	explicit LinuxSystemScheduler(Logs& logs) : m_logs(logs) {}
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() const override;
	std::optional<bool> status() const override;
	bool start() const override;
	bool disable() const override;
};

