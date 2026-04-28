#include "WindowsSystemScheduler.hpp"

WindowsSystemScheduler::WindowsSystemScheduler(Logs& logs) : m_logs(logs){
	// Init COM
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void**)&pService);
	// Connect to server
	pService->Connect(VARIANT(), VARIANT(), VARIANT(), VARIANT());
	// Get root folder
	pService->GetFolder(_bstr_t(L"\\").c_str(), &pFolder);
}

bool WindowsSystemScheduler::create() {
	ITaskDefinition* pTask = nullptr;
	pService->NewTask(0, &pTask);
    pTask->SetApplicationName(L"Rwal");
    pTask->SetWorkingDirectory(L"");
    pTask->SetPriority(1);

	ITaskActions* pActions = nullptr;
	pTask->get_Actions(&pActions);

	IExecAction* pExecAction = nullptr;
    pActions->Create(L"Exec", &pExecAction);
	pExecAction->SetPath(L"rwal.exe");
    pExecAction->SetArguments(L"change");
    pExecAction->Release();
}

bool WindowsSystemScheduler::start() const {
	IRegisteredTask* pTask = nullptr;
	if (pFolder->GetTask(L"Rwal", &pTask) == S_OK) {
		TASK_STATE state;
		if (pTask->get_State(&state) == S_OK && state == TASK_STATE_DISABLED) {
			return false;
		}
		pTask->Release();
    }
    return true;
}

bool WindowsSystemScheduler::reload() const {
    IRegisteredTask* pTask = nullptr;
    if (pFolder->GetTask(L"Rwal", &pTask) == S_OK) {
        pTask->Delete(0);
        pTask->Release();
    }
    return true;
}

bool WindowsSystemScheduler::disable() const {
    IRegisteredTask* pTask = nullptr;
    if (pFolder->GetTask(L"Rwal", &pTask) == S_OK) {
        pTask->Disable(0);
        pTask->Release();
    }
    return true;
}

std::string WindowsSystemScheduler::get() const {
	TaskTiming getTiming("Rwal", TaskScheduleType::None); {
    TaskTiming info;
    IRegisteredTask* pTask = nullptr;
    
    if (pRootFolder->GetTask(_bstr_t(name.c_str()), &pTask) != S_OK) return info;

    ITaskDefinition* pDef = nullptr;
    pTask->get_Definition(&pDef);

    ITriggerCollection* pTriggers = nullptr;
    pDef->get_Triggers(&pTriggers);

    long count = 0;
    pTriggers->get_Count(&count);

    if (count > 0) {
        ITrigger* pTrigger = nullptr;
        pTriggers->get_Item(1, &pTrigger);

        BSTR start;
        pTrigger->get_StartBoundary(&start);
        info.startBoundary = start;
        SysFreeString(start);

        TASK_TRIGGER_TYPE2 type;
        pTrigger->get_Type(&type);

        if (type == TASK_TRIGGER_DAILY) info.type = TaskScheduleType::Daily;
        else if (type == TASK_TRIGGER_TIME) info.type = TaskScheduleType::Hourly;
        
        pTrigger->Release();
    }

    pTriggers->Release();
    pDef->Release();
    pTask->Release();
    return info;
}

std::string WindowsSystemScheduler::set(const std::string& value) {
	IRegisteredTask* pTask = nullptr;
    if (pRootFolder->GetTask(_bstr_t("Rwal", &pTask) != S_OK) return;

    ITaskDefinition* pDef = nullptr;
    pTask->get_Definition(&pDef);

    ITriggerCollection* pTriggers = nullptr;
    pDef->get_Triggers(&pTriggers);
    pTriggers->Clear();

    if (type != TaskScheduleType::None) {
        ITrigger* pTrigger = nullptr;
        
        if (type == TaskScheduleType::Daily) {
            pTriggers->Create(TASK_TRIGGER_DAILY, &pTrigger);
            IDailyTrigger* pDaily = nullptr;
            pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDaily);
            pDaily->put_DaysInterval(1);
            pDaily->Release();
        } 
        else if (type == TaskScheduleType::Hourly) {
            pTriggers->Create(TASK_TRIGGER_TIME, &pTrigger);
            IRepetitionPattern* pRep = nullptr;
            pTrigger->get_Repetition(&pRep);
            pRep->put_Interval(_bstr_t(L"PT1H"));
            pRep->Release();
        }

        pTrigger->put_StartBoundary(_bstr_t(startTime.c_str()));
        pTrigger->Release();
    }

    IRegisteredTask* pUpdatedTask = nullptr;
    pRootFolder->RegisterTaskDefinition(_bstr_t(name.c_str()), pDef, TASK_UPDATE, 
        _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pUpdatedTask);

    if (pUpdatedTask) pUpdatedTask->Release();
    pTriggers->Release();
    pDef->Release();
    pTask->Release();
}

