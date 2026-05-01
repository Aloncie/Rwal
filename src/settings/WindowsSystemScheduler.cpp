#include "WindowsSystemScheduler.hpp"
#include "internal/AppConstants.hpp"
#include "SchedulerTypes.hpp"

WindowsSystemScheduler::WindowsSystemScheduler(Logs& logs) : m_logs(logs){
	// Init COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to init COM");
	}
	hr = CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void**)&pServicePtr);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to create COM instance");
	}
	// Connect to server
	pService->Connect(VARIANT(), VARIANT(), VARIANT(), VARIANT());
	// Get root folder
	pService->GetFolder(_bstr_t(L"\\").c_str(), &pFolderPtr);
}

WindowsSystemScheduler::~WindowsSystemScheduler() {
	CoUninitialize();
}

bool WindowsSystemScheduler::create() {
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

bool WindowsSystemScheduler::start() const {
	IRegisteredTaskPtr pTask;
	if (pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask) == S_OK) {
		TASK_STATE state;
		if (pTask->get_State(&state) == S_OK && state == TASK_STATE_DISABLED) {
			return false;
		}
    }
    return true;
}

bool WindowsSystemScheduler::reload() const {
    IRegisteredTaskPtr pTask;
	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (pTask != nullptr) {
		if (SUCCEEDED(hr)){
			pTask->Delete(0);
			return true;
		}
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to reload task");
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Task not found");
	return false;
}

bool WindowsSystemScheduler::disable() const {
	if (m_pFolder == nullptr) return false;
    IRegisteredTaskPtr pTask;
	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (pTask != nullptr) {
		if (SUCCEEDED(hr){
			hr = pTask->put_Enabled(VARIANT_FALSE);
			return SUCCEEDED(hr);
		}
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to disable task");
		return false;
    }
	m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Task not found");
    return false;
}

std::string WindowsSystemScheduler::get() const {
    IRegisteredTaskPtr pTask;
	// TaskScheduleType is enum in SchedulerTypes.hpp. It has all of the options: None, Hourly, Daily
	rwal::ui::Scheduler::TaskScheduleType TaskType = rwal::ui::Scheduler::TaskScheduleType::None;

	HRESULT hr = pFolder->GetTask(rwal::constants::names::WIN_TASK_NAME, &pTask);
    if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Task not found");
		return "Not found";
	}
    ITaskDefinitionPtr pDef;
    hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to get task definition");
		return "Error";
	}

    ITriggerCollectionptr pTriggers;
    hr = pDef->get_Triggers(&pTriggers);
	if (FAILED(hr)){
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to get triggers");
		return "Error";
	}

    long count = 0;
    hr = pTriggers->get_Count(&count);
	if (FAILED(hr)){
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to get trigger count");
        return "Error";
    }

    if (count > 0) {
        ITriggerPtr pTrigger;
        hr = pTriggers->get_Item(1, &pTrigger);
		if (FAILED(hr)){
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to get trigger");
			return "Error";
		}

        TASK_TRIGGER_TYPE2 type;
        hr = pTrigger->get_Type(&type);
		if (FAILED(hr)){
			m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to get trigger type");
			return "Error";
		}

		// There are only 3 types of triggers now so using if-else is good approach.
		// It will has more elegant solution later when this logic will be duplicated in another place or become hard mountain
        if (type == TASK_TRIGGER_DAILY) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Daily trigger found");
			TaskType = TaskScheduleType::Daily;
		}
        else if (type == TASK_TRIGGER_TIME) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Hourly trigger found");
			TaskType = TaskScheduleType::Hourly;
		} 
		else if (type == TASK_TRIGGER_NONE) {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "None trigger found");
			TaskType = TaskScheduleType::None;	
		} else {
			m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "Unknown trigger type");
		}
    } else{
		m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Scheduler, "No triggers found");
		return "Not found";
	}
    return rwal::ui::Scheduler::toString(TaskType);
}

std::string WindowsSystemScheduler::set(const std::string& value) {
	IRegisteredTaskPtr pTask;
    if (pRootFolder->GetTask(_bstr_t(rwal::constants::names::WIN_TASK_NAME, &pTask) != S_OK) return;

    ITaskDefinitionPtr pDef;
    pTask->get_Definition(&pDef);
	
    ITriggerCollectionPtr pTriggers;
    pDef->get_Triggers(&pTriggers);
    pTriggers->Clear();
	
	TaskScheduleType type = rwal::ui::Scheduler::tyType(value);

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
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Scheduler, "Failed to update task");
		return "Failed to set scheduler";
	}
}

