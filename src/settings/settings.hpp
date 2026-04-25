#pragma once
#include "logs/logs.hpp"

#include <string>
#include <filesystem>
#include <QDir>
#include <optional>

namespace fs = std::filesystem;

class AppSettings{
public:
	bool isCurcorVisible() const;
	void switchCursorVisiable();
};

class LinuxSystemScheduler : public ISystemScheduler{
private:
	std::optional<fs::path> getLocation() const;
	Logs& m_logs;
public:
	Timer(Logs& logs) : m_logs(logs) override {}
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() const override;
	bool status() const override;
	bool start() const override;
	bool disable() const override;
};

