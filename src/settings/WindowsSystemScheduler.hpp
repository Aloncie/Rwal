#pragma once
#include "logs/logs.hpp"
#include "ISystemScheduler.hpp"

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
_COM_SMARTPTR_TYPEDEF(ITrigget, __uuidof(ITriger));
_COM_SMARTPTR_TYPEDEF(IExecAction, __uuidof(IExecAction));
_COM_SMARTPTR_TYPEDEF(IDailyTrigget, __uuidof(IDailyTrigger));
_COM_SMARTPTR_TYPEDEF(ITaskActions, __uuidof(ITaskActions));

class WindowsSystemScheduler : public ISystemScheduler {
private:
	// Class smart pointers for COM interfaces.
	ITaskServicePtr m_pService;
	ITaskFolderPtr m_pFolder;
	Logs& m_logs;
public:
    explicit WindowsSystemScheduler(Logs& logs) : m_logs(logs) {}
	~WindowsSystemScheduler() override;
	std::string get() const override;
	std::string set(const std::string& value) override;
protected:
	bool create() override;
	bool reload() const override;
	bool start() const override;
	bool disable() const override;
	std::optional<bool> status() const override;
};
