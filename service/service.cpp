// service.cpp
#include "service.hpp"


SERVICE_STATUS        CurrencyService::g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE CurrencyService::g_StatusHandle = NULL;
bool                  CurrencyService::m_Paused = false;
bool                  CurrencyService::m_Running = true;

void CurrencyService::Start(DWORD dwArgc, LPWSTR* pszArgv) {
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // main loop
    while (m_Running) {
        Sleep(g_Config.interval);

        if (m_Paused) {
            continue;
        }

        std::string url = "/NBUStatService/v1/statdirectory/exchange";

        if (g_Config.dataFormat != "XML")
            url += "?json";

        auto data = getApiData(url);
        
        if (data == "") {
            g_Logger.Error(L"Something went wrong with the HTTP request.");
            continue;
        }

        std::string result;

        if (g_Config.dataFormat == "JSON" || g_Config.dataFormat == "CSV") {
            if (!validateJSON(data)) {
                g_Logger.Error(L"Malformed JSON");
                continue;
            }
            if (g_Config.dataFormat == "CSV")
                result = jsonToCSV(data);
            else
                result = data;
        }
        else { //xml
            if (!validateXML(data)) {
                g_Logger.Error(L"Malformed XML");
                continue;
            }
            result = data;
        }

        std::ofstream out(g_Config.dataFilePath, std::ios::out | std::ios::trunc);

        if (out.is_open()) {
            out << result;
            g_Logger.Info(L"Updated data.json");
            out.close();
        }
        else {
            g_Logger.Error(L"Couldn't write to destination");
        }
    }
}

void CurrencyService::Stop() {
    m_Running = false;

    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

void WINAPI CurrencyService::ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) {
    g_StatusHandle = RegisterServiceCtrlHandler(L"CurrencyService", ServiceCtrlHandler);
    if (g_StatusHandle == NULL) {
        return;
    }

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    Start(dwArgc, lpszArgv);
}

void WINAPI CurrencyService::ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_PAUSE:
        m_Paused = true;
        g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        break;

    case SERVICE_CONTROL_CONTINUE:
        m_Paused = false;
        g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        break;

    case SERVICE_CONTROL_STOP:
        Stop();
        break;
    default:
        break;
    }
}
