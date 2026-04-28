#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"

#include <optional>
#include <string>
#include <taskschd.h>

class WindowsSystemScheduler : public ISystemScheduler {
private:
	ITaskService* pService = nullptr;
	ITaskFolder* pFolder = nullptr;
public:
    explicit WindowsSystemScheduler(Logs& logs) : m_logs(logs) {}
	~WindowsSystemScheduler() override {};
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() const override;
	bool start() const override;
	bool disable() const override;
	std::optional<bool> status() const override;
};
