#include "WindowsSystemSchedule.hpp"
#include "internal/AppConstants.hpp"
#include "ScheduleTypes.hpp"

WindowsSystemSchedule::WindowsSystemSchedule(Logs& logs) : m_logs(logs){
	HRESULT hr = m_comguard.initResult();
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to initialize COM: " + std::string(ex.what()));
		return;
	}

	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&m_pService);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create COM instance");
		return;
	}
	// Connect to server
	hr = m_pService->Connect(VARIANT(), VARIANT(), VARIANT(), VARIANT());
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to connect to server");
		return;
	}
	// Get root folder
	hr = m_pService->GetFolder(_bstr_t(L"\\"), &m_pFolder);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get root folder");
		return;
	}
	if (m_pFolder == nullptr || m_pService == nullptr) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get root folder or service");
		return;
	}
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Schedule initialized");
}

bool WindowsSystemSchedule::status() const {
	IRegisteredTaskPtr pTask;
	HRESULT hr = m_pFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), &pTask);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task status");
		return false;
	}
	VARIANT_BOOL enabled;
	pTask->get_Enabled(&enabled);
	return enabled == VARIANT_TRUE;
}

bool WindowsSystemSchedule::create() {
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Try to create task");
	ITaskDefinitionPtr pTask;
	
	HRESULT hr = m_pService->NewTask(0, &pTask);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create task");
		return false;
	}
	
	IActionCollectionPtr pActions;
	hr = pTask->get_Actions(&pActions);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task actions");
		return false;
	}

	IExecActionPtr pExecAction;
    hr = pActions->Create(TASK_ACTION_EXEC, &pExecAction);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create exec action");
		return false;
	}
	hr = pExecAction->put_Path(_bstr_t(rwal::constants::files::EXEC_FILE));
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to set exec action path");
		return false;
	}
    hr = pExecAction->put_Arguments(_bstr_t(L"change"));
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to set exec action arguments");
		return false;
	}

	IRegisteredTaskPtr pRegisteredTask;

	hr = m_pFolder->RegisterTaskDefinition(
		_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()),
		pTask,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(),
		&pRegisteredTask
	);

	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to register task");
		return false;
	}
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task created successfully");
	return true;
}

bool WindowsSystemSchedule::start() const {
	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Try to start task");
	IRegisteredTaskPtr pTask;
	HRESULT hr = m_pFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), &pTask);
	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task");
		return false;
	}
	VARIANT_BOOL enabled;
	hr = pTask->get_Enabled(&enabled);

	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to check task state");
		return false;
	} 
	if (enabled == VARIANT_TRUE) {
        m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task already enabled");
        return true;
    }
	
	hr = pTask->put_Enabled(VARIANT_TRUE);
	HRESULT thr = pTask->get_Enabled(&enabled);
	if (FAILED(thr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to check enabled task status");
		return false;
	}
	if (FAILED(hr) || enabled != VARIANT_TRUE) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to enable task");
		return false;
	}

	m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task enabled successfully");
	return true;
}

bool WindowsSystemSchedule::reload() {
    IRegisteredTaskPtr pTask;
	HRESULT hr = m_pFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), &pTask);
    if (pTask != nullptr) {
		if (SUCCEEDED(hr)){
			hr = m_pFolder->DeleteTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), 0);
			if (FAILED(hr)) {
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to delete task");
				return false;
			}
			if (!create()){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create task");
				return false;
			}
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task reloaded successfully");
			return true;
		}
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to reload task");
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Task not found");
	return false;
}

bool WindowsSystemSchedule::disable() const {
    IRegisteredTaskPtr pTask;
	HRESULT hr = m_pFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), &pTask);

    if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Task not found");
		return false;
	}

	hr = pTask->put_Enabled(VARIANT_FALSE);
	VARIANT_BOOL enabled;
	HRESULT thr = pTask->get_Enabled(&enabled);

	if (FAILED(hr)) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to disable task");
		return false;
	}

	if (FAILED(thr) || enabled != VARIANT_FALSE) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to check task state");
		return false;
	}
	return true;
}

std::string WindowsSystemSchedule::get() const {
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Schedule, "Try to get task schedule");
	auto TaskType = rwal::system::Schedule::TaskScheduleType::None;
	// Check for the disabled task before any other actions.
	auto statusInput = status();
	if (statusInput == std::nullopt){
		return "Error";
	}
	if (!statusInput.value()){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task isn't active");
		return "None";
	}

	auto triggersInput = getTaskTriggers();
	if (triggersInput == std::nullopt) return "Error";
	ITriggerCollectionPtr pTriggers = triggersInput.value();

    long count = 0;
    HRESULT hr = pTriggers->get_Count(&count);
	if (FAILED(hr)){
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get trigger count");
        return "Error";
    }
	
    if (count > 0) {
        ITriggerPtr pTrigger;
        hr = pTriggers->get_Item(1, &pTrigger);
		if (FAILED(hr)){
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get trigger");
			return "Error";
		}

        TASK_TRIGGER_TYPE2 type;
        hr = pTrigger->get_Type(&type);
		if (FAILED(hr)){
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get trigger type");
			return "Error";
		}

		// There are only 3 types of triggers now so using if-else is good approach.
		// It will has more elegant solution later when this logic will be duplicated in another place or become hard mountain
        if (type == TASK_TRIGGER_DAILY) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Daily trigger found");
			TaskType = TaskScheduleType::Daily;
		}
        else if (type == TASK_TRIGGER_TIME) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Hourly trigger found");
			TaskType = TaskScheduleType::Hourly;
		} else {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Unknown trigger type");
			return "Unknown";
		}
    } else{
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "No triggers found");
		return "Not found";
	}
    return rwal::system::Schedule::toString(TaskType);
}

std::string WindowsSystemSchedule::set(const std::string& value) {
	using namespace rwal::system::Schedule;
	const std::string failedLog = "Failed set task. More info in logs.";
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Schedule, "Try to set task");

	auto triggersInput = getTaskTriggers();
	if (triggersInput == std::nullopt) return "Error";

	ITriggerCollectionPtr pTriggers = triggersInput.value();

    HRESULT hr = pTriggers->Clear();
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to clear triggers");
		return failedLog;
	}
	
	TaskScheduleType type = toType(value);

	// There are only 3 types of triggers now so using if-else is good approach.
	// It will has more elegant solution later 
	// when this logic will be duplicated in another place or become hard mountain
    if (type != TaskScheduleType::None) {
        ITriggerPtr pTrigger;
        
        if (type == TaskScheduleType::Daily) {
            hr = pTriggers->Create(TASK_TRIGGER_DAILY, &pTrigger);
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create daily trigger");
				return failedLog;
			}
            IDailyTriggerPtr pDaily;
            hr = pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDaily);
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get daily trigger interface");
				return failedLog;
			}
            hr = pDaily->put_DaysInterval(1);
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to set daily trigger interval");
				return failedLog;
			}
        } 
        else if (type == TaskScheduleType::Hourly) {
            hr = pTriggers->Create(TASK_TRIGGER_TIME, &pTrigger);
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create hourly trigger");
				return failedLog;
			}
            IRepetitionPatternPtr pRep;
            hr = pTrigger->get_Repetition(&pRep);
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get repetition pattern");
				return failedLog;
			}
            hr = pRep->put_Interval(_bstr_t(L"PT1H"));
			if (FAILED(hr)){
				m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to set repetition pattern interval");
				return failedLog;
			}
        }
    }

    IRegisteredTaskPtr pUpdatedTask;
	auto definitionInput = getTaskDefinition();
	if (definitionInput == std::nullopt) return "Error";

	ITaskDefinitionPtr pDef = definitionInput.value();
    hr = m_pFolder->RegisterTaskDefinition(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), pDef, TASK_UPDATE, 
        _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pUpdatedTask);
	if (FAILED(hr)){
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to update task");
        return failedLog;
    }
	return "Task updated successfully";
}

std::optional<ITriggerCollectionPtr> WindowsSystemSchedule::getTaskTriggers() const {
	auto definitionInput = getTaskDefinition();
	if (definitionInput == std::nullopt) return std::nullopt;

	ITaskDefinitionPtr pDef = definitionInput.value();
    ITriggerCollectionPtr pTriggers;
    HRESULT hr = pDef->get_Triggers(&pTriggers);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get triggers");
		return std::nullopt;
	}
	return pTriggers;
}

std::optional<ITaskDefinitionPtr> WindowsSystemSchedule::getTaskDefinition() const {
	IRegisteredTaskPtr pTask;

	HRESULT hr = m_pFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME.data()), &pTask);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task");
		return std::nullopt;
	}

    ITaskDefinitionPtr pDef;
    hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task definition");
		return std::nullopt;
	}
	return pDef;
}

