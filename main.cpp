#include <iostream>
#include <cstdio>


#include "service/service.hpp"
#include "service/manager.hpp"



int main(int argc, char* argv[]) {
    auto configPath = getExecutableDir() + "\\config.json";
    
    // failed to load config
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


    // console commands
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
        else if (arg == "--update") {
            SendCommand(serviceName, SERVICE_CONTROL_UPDATE);
        }
        else {
            std::cerr << "Invalid argument. Use --install or --uninstall.\n";
            return 1;
        }
        return 0;
    }

    // If no console window attached - we are a service, otherwise proceed in console mode
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
        std::cout << "Choose an option:" << std::endl;
        std::cout << "1. Install service" << std::endl;
        std::cout << "2. Uninstall service" << std::endl;
        std::cout << "3. Update config" << std::endl;
        std::cout << "4. Exit" << std::endl;

        int choice;
        std::cin >> choice;
        std::cin.ignore();  // To consume the newline character after the number

        switch (choice) {
        case 1:
            wchar_t servicePath[MAX_PATH] = { 0 };
            GetModuleFileName(NULL, servicePath, MAX_PATH);

            if (!IsInstalled(serviceName))
                InstallService(serviceName, servicePath);
            else
                std::cout << "Service is already installed." << std::endl;
            break;
        case 2:
            if (IsInstalled(serviceName))
                UninstallService(serviceName);
            else
                std::cout << "Service is not installed." << std::endl;
            break;
        case 3:
            if (IsInstalled(serviceName)) 
                SendCommand(serviceName, SERVICE_CONTROL_UPDATE);
            else 
                std::cout << "Service is not installed." << std::endl;
            break;
        case 4:
            std::cout << "Exiting..." << std::endl;
            return 0;
        default:
            std::cout << "Invalid option. Please run the program again and select a valid option." << std::endl;
            break;
        }

        std::cout << "Press any key to exit." << std::endl;
        std::cin.get();

        return 0;
    }
    return 0;
}
