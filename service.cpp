// service.cpp
#include "service.hpp"
#include <iostream>

SERVICE_STATUS        CurrencyService::g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE CurrencyService::g_StatusHandle = NULL;

void CurrencyService::Start(DWORD dwArgc, LPWSTR* pszArgv) {
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // Here would be your service's actual logic, e.g., a loop, thread, etc.
}

void CurrencyService::Stop() {
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

void WINAPI CurrencyService::ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) {
    g_StatusHandle = RegisterServiceCtrlHandler(L"SampleService", ServiceCtrlHandler);
    if (g_StatusHandle == NULL) {
        return;
    }

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    Start(dwArgc, lpszArgv);
}

void WINAPI CurrencyService::ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        Stop();
        break;
    default:
        break;
    }
}
