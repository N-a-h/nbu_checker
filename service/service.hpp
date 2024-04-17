// service.hpp
#pragma once
 
#include "config.hpp"
#include "logger.hpp"

class CurrencyService {
public:
    static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv);
    static void WINAPI ServiceCtrlHandler(DWORD dwControl);

private:
    static bool m_Paused;
    static bool m_Running;
    static SERVICE_STATUS          g_ServiceStatus;
    static SERVICE_STATUS_HANDLE   g_StatusHandle;
    
    static void Start(DWORD dwArgc, LPWSTR* pszArgv);
    static void processExchangeData();
    static bool validateDataFormat(const std::string& data);
    static bool writeDataToFile(const std::string& data);
    static void Stop();
};
