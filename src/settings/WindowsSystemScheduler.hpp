#pragma once
#include "ISystemScheduler.hpp"
#include "SchedulerTypes.hpp"

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
	ComGuard() {
		m_initResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}
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

class WindowsSystemScheduler : public ISystemScheduler {
private:
	// Guard class for COM initialization and uninitialization for any way of exception.
	ComGuard m_comguard;
	// Class smart pointers for COM interfaces.
	ITaskServicePtr m_pService;
	ITaskFolderPtr m_pFolder;
	
	// use optional for these methods to separate 'no data' from 'failure'
	std::optional<ITriggerCollectionPtr> getTaskTriggers() const;
	std::optional<ITaskDefinitionPtr> getTaskDefinition() const;
public:
    WindowsSystemScheduler(Logs& logs, IFileSystem& fs);
	~WindowsSystemScheduler() override = default;
	std::optional<TaskSchedulerType> get() const override;
	std::string set(TaskSchedulerType type) override;
protected:
	bool create() override;
	bool reload() override;
	bool start() const override;
	bool disable() const override;
	bool status() const override;
};

