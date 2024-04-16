#include "installer.hpp"


void InstallService(const wchar_t* serviceName, wchar_t* servicePath) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return;
    }

    SC_HANDLE schService = CreateService(
        schSCManager,           
        serviceName,            
        serviceName,            
        SERVICE_ALL_ACCESS,     
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,    
        SERVICE_ERROR_NORMAL,  
        servicePath,           
        NULL,                  
        NULL,                  
        NULL,                   
        NULL,                   
        NULL);                  

    if (schService == NULL) {
        std::wcerr << L"CreateService failed with error: " << GetLastError() << std::endl;
    }
    else {
        std::wcout << L"Service installed successfully\n";
        CloseServiceHandle(schService);
    }

    CloseServiceHandle(schSCManager);
}

void UninstallService(const wchar_t* serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_STOP | DELETE);
    if (schService == NULL) {
        std::wcerr << L"OpenService failed with error: " << GetLastError() << std::endl;
    }
    else {
        // Try to stop the service
        SERVICE_STATUS ss;
        if (ControlService(schService, SERVICE_CONTROL_STOP, &ss)) {
            std::wcout << L"Stopping " << serviceName << std::endl;
            Sleep(1000);

            while (QueryServiceStatus(schService, &ss)) {
                if (ss.dwCurrentState == SERVICE_STOP_PENDING) {
                    Sleep(1000);
                }
                else {
                    break;
                }
            }

            std::wcout << serviceName << L" stopped\n";
        }

        // Now remove the service
        if (DeleteService(schService)) {
            std::wcout << serviceName << L" removed\n";
        }
        else {
            std::wcerr << L"DeleteService failed with error: " << GetLastError() << std::endl;
        }

        CloseServiceHandle(schService);
    }

    CloseServiceHandle(schSCManager);
}

bool IsInstalled(const wchar_t* serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return false;
    }

    // Try to open the service
    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST) {
            std::wcout << L"Service is not installed.\n";
            CloseServiceHandle(schSCManager);
            return false;
        }
        std::wcerr << L"OpenService failed with error: " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    std::wcout << L"Service is already installed.\n";
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}
