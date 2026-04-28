#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"

#include <optional>
#include <string>
#include <taskschd.h>
#include <comdef.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsuppw.lib")

_COM_SMARTPTR_TYPEDEF(ITaskService, __uuidof(ITaskService));

class WindowsSystemScheduler : public ISystemScheduler {
private:
	ITaskService m_pService;
	ITaskFolder m_pFolder;
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
