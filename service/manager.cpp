#include "manager.hpp"


/// <summary>
/// Installs the service
/// </summary>
/// <param name="serviceName">service name</param>
/// <param name="servicePath">path to the service executable</param>
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
/// <summary>
/// Starts the service.
/// </summary>
/// <param name="serviceName">The name of the service to start.</param>
/// <returns>true if the service is started successfully, false otherwise.</returns>
bool StartUserService(const wchar_t* serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_START);
    if (schService == NULL) {
        std::wcerr << L"OpenService failed with error: " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    if (!::StartService(schService, 0, NULL)) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_ALREADY_RUNNING) {
            std::wcout << L"The service is already running.\n";
        }
        else {
            std::wcerr << L"StartService failed with error: " << error << std::endl;
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return false;
        }
    }
    else {
        std::wcout << L"Service started successfully.\n";
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}
/// <summary>
/// Uninstalls the service
/// </summary>
/// <param name="serviceName"></param>
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



/// <summary>
/// Checks whether the service is already installed
/// </summary>
/// <param name="serviceName">service name</param>
/// <returns>true if installed</returns>
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


/// <summary>
/// Sends a custom command to a Windows service.
/// </summary>
/// <param name="serviceName">The name of the service to which the command will be sent.</param>
/// <param name="serviceCommand">The custom command identifier, must be between 128 and 255.</param>
/// <returns>true if the command was successfully sent, false otherwise.</returns>
bool SendCustomCommand(const wchar_t* serviceName, DWORD serviceCommand) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_USER_DEFINED_CONTROL);
    if (schService == NULL) {
        std::wcerr << L"OpenService failed with error: " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    if (!ControlService(schService, serviceCommand, (LPSERVICE_STATUS)&ssStatus)) {
        std::wcerr << L"ControlService failed with error: " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    std::wcout << L"Command sent successfully to " << serviceName << std::endl;
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}


/// <summary>
/// Sends a control command to the service
/// </summary>
/// <param name="serviceName">The name of the service to control.</param>
/// <param name="controlCommand">The command to send (e.g., SERVICE_CONTROL_STOP, SERVICE_CONTROL_PAUSE, SERVICE_CONTROL_CONTINUE).</param>
/// <returns>true if the command was successfully sent</returns>
bool SendControlCommand(const wchar_t* serviceName, DWORD controlCommand) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"OpenSCManager failed with error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        std::wcerr << L"OpenService failed with error: " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!ControlService(schService, controlCommand, (LPSERVICE_STATUS)&ssp)) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_CANNOT_ACCEPT_CTRL) {
            std::wcerr << L"Service cannot accept the control command at this time." << std::endl;
        }
        else {
            std::wcerr << L"ControlService failed with error: " << error << std::endl;
        }
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    std::wcout << L"Command sent successfully to " << serviceName << std::endl;
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}
