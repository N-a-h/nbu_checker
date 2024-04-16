// service.hpp
#pragma once

#include <windows.h>

class CurrencyService {
public:
    static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv);
    static void WINAPI ServiceCtrlHandler(DWORD dwControl);

private:
    static SERVICE_STATUS          g_ServiceStatus;
    static SERVICE_STATUS_HANDLE   g_StatusHandle;
    static void Start(DWORD dwArgc, LPWSTR* pszArgv);
    static void Stop();
};
