#pragma once
#include "logs/logs.hpp"
#include "ISystemSchedule.hpp"
#include "internal/AppConstants.hpp"

#include <optional>
#include <string>
#include <taskschd.h>
#include <comdef.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsuppw.lib")

// Use smart pointers for COM to avoid memory leaks in class and methods variables.
//
// Don't use memory management manually(Release() method) because it can cause memory leak or crash app.
//
// For methods - declare them before methods body with class and 'Ptr' postfix. 
// E.g. use 'ITaskServicePtr' instead of a pointer to ITaskService interface.

// Declare smart pointers for COM interfaces.
_COM_SMARTPTR_TYPEDEF(ITaskService, __uuidof(ITaskService));
_COM_SMARTPTR_TYPEDEF(ITaskFolder, __uuidof(ITaskFolder));
_COM_SMARTPTR_TYPEDEF(IRegisteredTask, __uuidof(IRegisteredTask));
_COM_SMARTPTR_TYPEDEF(ITriggerCollection, __uuidof(ITriggerCollection));
_COM_SMARTPTR_TYPEDEF(ITaskDefinition, __uuidof(ITaskDefinition));
_COM_SMARTPTR_TYPEDEF(ITrigger, __uuidof(ITrigger));
_COM_SMARTPTR_TYPEDEF(IExecAction, __uuidof(IExecAction));
_COM_SMARTPTR_TYPEDEF(IDailyTrigger, __uuidof(IDailyTrigger));
_COM_SMARTPTR_TYPEDEF(IActionCollection, __uuidof(IActionCollection));
_COM_SMARTPTR_TYPEDEF(IRepetitionPattern, __uuidof(IRepetitionPattern));

class ComGuard{
	HRESULT m_initResult;
public:
	ComGuard() : m_initResult(CoInitializeEx(nullptr, COINIT_MULTITHREADED)) {}
	~ComGuard(){
		if (SUCCEEDED(m_initResult)) CoUninitialize();
	}

	HRESULT initResult() const { return m_initResult; }
	
	// Disallow copying.
	// Needed to ensure the existence of a single object.
	// Can lead to undefined behavior when using more than one object.
	ComGuard(const ComGuard&) = delete;
    ComGuard& operator=(const ComGuard&) = delete;
};

class WindowsSystemSchedule : public ISystemSchedule {
private:
	// Guard class for COM initialization and uninitialization for any way of exception.
	ComGuard m_comguard;
	// Class smart pointers for COM interfaces.
	ITaskServicePtr m_pService;
	ITaskFolderPtr m_pFolder;

	Logs& m_logs;

	std::optional<ITriggerCollectionPtr> getTaskTriggers() const;
	std::optional<ITaskDefinitionPtr> getTaskDefinition() const;
public:
    explicit WindowsSystemSchedule(Logs& logs) : m_logs(logs);
	~WindowsSystemSchedule() override = default;
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() override;
	bool start() const override;
	bool disable() const override;
	std::optional<bool> status() const override;
};
