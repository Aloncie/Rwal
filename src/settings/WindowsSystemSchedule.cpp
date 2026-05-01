#include "WindowsSystemSchedule.hpp"
#include "internal/AppConstants.hpp"
#include "ScheduleTypes.hpp"

WindowsSystemSchedule::WindowsSystemSchedule(Logs& logs) : m_logs(logs){
	// Init COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to init COM");
	}
	hr = CoCreateInstance(CLSID_CTaskSchedule, NULL, CLSCTX_INPROC_SERVER, IID_ITaskSchedule, (void**)&pServicePtr);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to create COM instance");
	}
	// Connect to server
	pService->Connect(VARIANT(), VARIANT(), VARIANT(), VARIANT());
	// Get root folder
	pService->GetFolder(_bstr_t(L"\\").c_str(), &pFolderPtr);
}

WindowsSystemSchedule::~WindowsSystemSchedule() {
	CoUninitialize();
}

bool WindowsSystemSchedule::create() {
	ITastDefinitionPtr pTask;

	pService->NewTask(0, &pTask);
    pTask->SetApplicationName(rwal::constants::names::WIN_TASK_NAME);
    pTask->SetWorkingDirectory(L"");
    pTask->SetPriority(1);
	
	ITaskActionsPtr pActions; 
	pTask->get_Actions(&pActions);

	IExecActionPtr pExecAction;
    pActions->Create(L"Exec", &pExecAction);
	pExecAction->SetPath(rwal::constants::files::EXE_FILE);
    pExecAction->SetArguments(L"change");
}

bool WindowsSystemSchedule::start() const {
	IRegisteredTaskPtr pTask;
	if (pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask) == S_OK) {
		TASK_STATE state;
		if (pTask->get_State(&state) == S_OK && state == TASK_STATE_DISABLED) {
			return false;
		}
    }
    return true;
}

bool WindowsSystemSchedule::reload() const {
    IRegisteredTaskPtr pTask;
	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (pTask != nullptr) {
		if (SUCCEEDED(hr)){
			pTask->Delete(0);
			return true;
		}
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to reload task");
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Task not found");
	return false;
}

bool WindowsSystemSchedule::disable() const {
	if (m_pFolder == nullptr) return false;
    IRegisteredTaskPtr pTask;
	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (pTask != nullptr) {
		if (SUCCEEDED(hr){
			hr = pTask->put_Enabled(VARIANT_FALSE);
			return SUCCEEDED(hr);
		}
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to disable task");
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Task not found");
    return false;
}

std::string WindowsSystemSchedule::get() const {
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Schedule, "Try to get task");
    IRegisteredTaskPtr pTask;
	rwal::ui::Schedule::TaskScheduleType TaskType = rwal::ui::Schedule::TaskScheduleType::None;

	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Task not found");
		return "Not found";
	}
    ITaskDefinitionPtr pDef;
    hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task definition");
		return "Error";
	}

    ITriggerCollectionptr pTriggers;
    hr = pDef->get_Triggers(&pTriggers);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get triggers");
		return "Error";
	}

    long count = 0;
    hr = pTriggers->get_Count(&count);
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
		} 
		else if (type == TASK_TRIGGER_NONE) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "None trigger found");
			TaskType = TaskScheduleType::None;	
		} else {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "Unknown trigger type");
		}
    } else{
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Schedule, "No triggers found");
		return "Not found";
	}
    return rwal::ui::Schedule::toString(TaskType);
}

std::string WindowsSystemSchedule::set(const std::string& value) {
	m_logs.writeLogs(rwal::logs::types::Debug, rwal::logs::modules::Schedule, "Try to set task");
	IRegisteredTaskPtr pTask;

	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task");
		return "Error";
	}

    ITaskDefinitionPtr pDef;
    hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get task definition");
		return "Error";
	}
	
    ITriggerCollectionPtr pTriggers;
    hr = pDef->get_Triggers(&pTriggers);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to get triggers");
		return "Error";
	}
    pTriggers->Clear();
	
	
	TaskScheduleType type = rwal::ui::Schedule::toType(value);

    if (type != TaskScheduleType::None) {
        ITriggerPtr pTrigger;
        
        if (type == TaskScheduleType::Daily) {
            pTriggers->Create(TASK_TRIGGER_DAILY, &pTrigger);
            IDailyTriggerPtr pDaily;
            pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDaily);
            pDaily->put_DaysInterval(1);
        } 
        else if (type == TaskScheduleType::Hourly) {
            pTriggers->Create(TASK_TRIGGER_TIME, &pTrigger);
            IRepetitionPatternPtr pRep;
            pTrigger->get_Repetition(&pRep);
            pRep->put_Interval(_bstr_t(L"PT1H"));
        }

        pTrigger->put_StartBoundary(_bstr_t(startTime.c_str()));
    }

    IRegisteredTaskPtr pUpdatedTask;
    pRootFolder->RegisterTaskDefinition(_bstr_t(name.c_str()), pDef, TASK_UPDATE, 
        _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pUpdatedTask);

    if (!pUpdatedTask){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Schedule, "Failed to update task");
		return "Failed to set scheduler";
	}
}

