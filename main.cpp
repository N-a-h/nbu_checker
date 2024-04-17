#include <iostream>
#include <cstdio>


#include "service/service.hpp"
#include "service/installer.hpp"

// service name
#define serviceName L"NbuChecker"

int main(int argc, char* argv[]) {
    auto configPath = getExecutableDir() + "\\config.json";
    
    // failed to save config
    if (!g_Config.loadFromFile(configPath)) {
        g_Config.writeDefaultConfig(configPath);
        if(!g_Config.Update()) {
            std::cout << "Failed to update config, exiting" << std::endl;
            return 1;
        }
    }

    if (!g_Config.isValid()) {
        std::cout << "The config is invalid, exiting" << std::endl;
        return 1;
    }


    if (argc > 1) {
        wchar_t servicePath[MAX_PATH] = { 0 };
        if (GetModuleFileName(NULL, servicePath, MAX_PATH) > 0) {
            std::wcout << L"The path to the current executable is: " << servicePath << std::endl;
        }
        else {
            std::wcerr << L"Failed to get the path. Error: " << GetLastError() << std::endl;
            return 1;
        }

        std::string arg = argv[1];
        if (arg == "--install") {
            InstallService(serviceName, servicePath);
        }
        else if (arg == "--uninstall") {
            UninstallService(serviceName);
        }
        else {
            std::cerr << "Invalid argument. Use --install or --uninstall.\n";
            return 1;
        }
        return 0;
    }

    //running as a service since we don't have a console window
    if (GetConsoleWindow() == NULL) {
        SERVICE_TABLE_ENTRY ServiceTable[] = {
            {(LPWSTR)L"CurrencyService", CurrencyService::ServiceMain},
            {NULL, NULL}
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
            return GetLastError();
        }
    }
    else {
        if (IsInstalled(serviceName)) {
            std::cout << "Run this executable again with --uninstall to uninstall the service" << std::endl;
        } 
        else {
            std::cout << "Run this executable again with --install to install the service" << std::endl;
        }
        std::cout << "Press any key to exit" << std::endl;
        std::cin.get();
    }
    return 0;
}
