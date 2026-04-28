#include "WindowsSystemScheduler.hpp"

WindowsSystemScheduler::WindowsSystemScheduler(Logs& logs) : m_logs(logs){
	// Init COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::WindowsSystemScheduler, "Failed to init COM");
	}
	hr = CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void**)&pServicePtr);
	if (hr != S_OK) {
		m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::WindowsSystemScheduler, "Failed to create COM instance");
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
    pTask->SetApplicationName(L"Rwal");
    pTask->SetWorkingDirectory(L"");
    pTask->SetPriority(1);
	
	ITaskActionsPtr pActions; 
	pTask->get_Actions(&pActions);

	IExecActionPtr pExecAction;
    pActions->Create(L"Exec", &pExecAction);
	pExecAction->SetPath(L"rwal.exe");
    pExecAction->SetArguments(L"change");
}

bool WindowsSystemScheduler::start() const {
	IRegisteredTaskPtr pTask;
	if (pFolder->GetTask(L"Rwal", &pTask) == S_OK) {
		TASK_STATE state;
		if (pTask->get_State(&state) == S_OK && state == TASK_STATE_DISABLED) {
			return false;
		}
    }
    return true;
}

bool WindowsSystemScheduler::reload() const {
    IRegisteredTaskPtr pTask;
    if (pFolder->GetTask(L"Rwal", &pTask) == S_OK) {
        pTask->Delete(0);
    }
    return true;
}

bool WindowsSystemScheduler::disable() const {
	if (m_pFolder == nullptr) return false;
    IRegisteredTaskPtr pTask;
	HRESULT hr = pFolder->GetTask(L"Rwal", &pTask);
    if (SUCCEEDED(hr) && pTask != nullptr) {
		hr = pTask->put_Enabled(VARIANT_FALSE);
		return SUCCEEDED(hr);
    }
    return false;
}

std::string WindowsSystemScheduler::get() const {
	TaskTiming getTiming("Rwal", TaskScheduleType::None); {
    TaskTiming info;
    IRegisteredTaskPtr pTask;
    
    if (pRootFolder->GetTask(_bstr_t(name.c_str()), &pTask) != S_OK) return info;

    ITaskDefinitionPtr pDef;
    pTask->get_Definition(&pDef);

    ITriggerCollectionptr pTriggers;
    pDef->get_Triggers(&pTriggers);

    long count = 0;
    pTriggers->get_Count(&count);

    if (count > 0) {
        ITriggerPtr pTrigger;
        pTriggers->get_Item(1, &pTrigger);

        BSTR start;
        pTrigger->get_StartBoundary(&start);
        info.startBoundary = start;
        SysFreeString(start);

        TASK_TRIGGER_TYPE2 type;
        pTrigger->get_Type(&type);

        if (type == TASK_TRIGGER_DAILY) info.type = TaskScheduleType::Daily;
        else if (type == TASK_TRIGGER_TIME) info.type = TaskScheduleType::Hourly;
    }

    return info;
}

std::string WindowsSystemScheduler::set(const std::string& value) {
	IRegisteredTaskPtr pTask;
    if (pRootFolder->GetTask(_bstr_t("Rwal", &pTask) != S_OK) return;

    ITaskDefinitionPtr pDef;
    pTask->get_Definition(&pDef);

    ITriggerCollectionPtr pTriggers;
    pDef->get_Triggers(&pTriggers);
    pTriggers->Clear();

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
            IRepetitionPattern* pRep = nullptr;
            pTrigger->get_Repetition(&pRep);
            pRep->put_Interval(_bstr_t(L"PT1H"));
        }

        pTrigger->put_StartBoundary(_bstr_t(startTime.c_str()));
    }

    IRegisteredTaskPtr pUpdatedTask;
    pRootFolder->RegisterTaskDefinition(_bstr_t(name.c_str()), pDef, TASK_UPDATE, 
        _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pUpdatedTask);

    if (!pUpdatedTask) return "Failed to update task";
}

