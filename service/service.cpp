// service.cpp
#include "service.hpp"


SERVICE_STATUS        CurrencyService::g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE CurrencyService::g_StatusHandle = NULL;
bool                  CurrencyService::m_Paused = false;
bool                  CurrencyService::m_Running = true;


/// <summary>
/// Main service loop
/// </summary>
void CurrencyService::Start(DWORD dwArgc, LPWSTR* pszArgv) {
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    while (m_Running) {
        if (!m_Paused) {
            CurrencyService::processExchangeData();
        }
        Sleep(g_Config.interval);
    }
}
/// <summary>
/// Fetches, processes and writes data
/// </summary>
void CurrencyService::processExchangeData() {
    std::string url = "/NBUStatService/v1/statdirectory/exchange";
    url += (g_Config.dataFormat != "XML") ? "?json" : "";

    std::string data = getApiData(url);
    if (data.empty()) {
        g_Logger.Error(L"Failed HTTP request.");
        return;
    }

    if (!CurrencyService::validateDataFormat(data)) {
        g_Logger.Error(L"Malformed data format.");
        return;
    }

    //convert json to csv before writing data
    if (g_Config.dataFormat == "CSV")
        data = jsonToCSV(data);

    //prettify json because the data nbu returns is ugly
    if (g_Config.dataFormat == "JSON")
        data = prettifyJSON(data);

    if (!writeDataToFile(data)) {
        g_Logger.Error(L"Couldn't write to file.");
    }
}

/// <summary>Validates fetched data depending on g_Config.</summary>
/// <param name="data">Data to be validated</param>
/// <returns>True on success, false on failure</returns>
bool CurrencyService::validateDataFormat(const std::string& data) {
    if (g_Config.dataFormat == "JSON" || g_Config.dataFormat == "CSV") {
        if (!validateJSON(data)) {
            g_Logger.Error(L"Malformed JSON data.");
            return false;
        }
    }
    else if (g_Config.dataFormat == "XML") {
        if (!validateXML(data)) {
            g_Logger.Error(L"Malformed XML data.");
            return false;
        }
    }
    else {
        g_Logger.Error(L"Unsupported data format specified.");
        return false;
    }
    return true;
}



/// <summary>Writes data to a file</summary>
/// <param name="data">Data to be written</param>
/// <returns>True on success, false on failure</returns> 
bool CurrencyService::writeDataToFile(const std::string& data) {
    std::ofstream out(g_Config.dataFilePath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        g_Logger.Error(L"Failed to open the output file.");
        return false;
    }

    out << data;
    if (out.fail()) {
        g_Logger.Error(L"Failed to write data to the output file.");
        out.close();
        return false;
    }

    out.close();
    g_Logger.Info(L"Data successfully written to file.");
    return true;
}

/// <summary>
/// Stops the service
/// </summary>
void CurrencyService::Stop() {
    m_Running = false;
    g_Logger.Warn(L"Stopping the service");
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

/// <summary>
/// Service entry point
/// </summary>
/// <param name="dwArgc"></param>
/// <param name="lpszArgv"></param>
/// <returns></returns>
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

/// <summary>
/// Service command handler
/// </summary>
/// <param name="dwControl">command</param>
/// <returns></returns>
void WINAPI CurrencyService::ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_UPDATE:
        if (g_Config.Update()) {
            if (g_Config.isValid())
                g_Logger.Info(L"Service config updated successfully");
            else {
                g_Logger.Warn(L"Config is invalid, stopping the service\nPlease rerun the manager executable for hints on fixing it");
                Stop();
            }
        }
        else {
            g_Logger.Error(L"Failed to open the config file. Stopping the service");
            Stop();
        }
        break;

    case SERVICE_CONTROL_PAUSE:
        m_Paused = true;
        g_Logger.Info(L"Service paused");
        g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        break;

    case SERVICE_CONTROL_CONTINUE:
        m_Paused = false;
        g_Logger.Info(L"Service unpaused");
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
