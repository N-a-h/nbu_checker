#include <iostream>
#include <cstdio>


#include "service/service.hpp"
#include "service/manager.hpp"

/// <summary>
/// Clears the console
/// </summary>
void clearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &screen);
    FillConsoleOutputCharacterA(
        hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(hConsole, topLeft);
}


/// <summary>
/// Prints an interactive menu to interact with the service
/// </summary>
/// <param name="serviceName"></param>
/// <param name="servicePath"></param>
void Menu(const wchar_t* serviceName, wchar_t* servicePath) {
    std::cout << "Choose an option:" << std::endl;
    std::cout << "1. Install service" << std::endl;
    std::cout << "2. Uninstall service" << std::endl;
    std::cout << "3. Start service" << std::endl;
    std::cout << "4. Stop service" << std::endl;
    std::cout << "5. Pause service" << std::endl;
    std::cout << "6. Continue service" << std::endl;
    std::cout << "7. Update config" << std::endl;
    std::cout << "8. Exit" << std::endl;

    int choice;
    std::cin >> choice;
    std::cin.ignore();

    switch (choice) {
    case 1:
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
        StartUserService(serviceName);
        break;
    case 4:
        SendControlCommand(serviceName, SERVICE_CONTROL_STOP);
        break;
    case 5:
        SendControlCommand(serviceName, SERVICE_CONTROL_PAUSE);
        break;
    case 6:
        SendControlCommand(serviceName, SERVICE_CONTROL_CONTINUE);
        break;
    case 7:
        SendCustomCommand(serviceName, SERVICE_CONTROL_UPDATE);
        break;
    case 8:
        std::cout << "Exiting..." << std::endl;
        exit(0);
    default:
        std::cout << "Invalid option." << std::endl;
        break;
    }

    std::cout << "Press any key to continue." << std::endl;
    std::cin.get();

    clearConsole();
}

int main(int argc, char* argv[]) {
    auto configPath = getExecutableDir() + "\\config.json";
    
    // failed to load config
    if (!g_Config.loadFromFile(configPath)) {
        g_Config.writeDefaultConfig(configPath);
        if(!g_Config.Update()) {
            std::cout << "Failed to update config, exiting" << std::endl;
            std::cout << "Press any key to exit." << std::endl;
            std::cin.get();
            return 1;
        }
    }

    if (!g_Config.isValid()) {
        std::cout << "The config is invalid, exiting" << std::endl;
        std::cout << "Press any key to exit." << std::endl;
        std::cin.get();
        return 1;
    }

    wchar_t servicePath[MAX_PATH] = { 0 };
    if (GetModuleFileName(NULL, servicePath, MAX_PATH) > 0) {
        std::wcout << L"The path to the current executable is: " << servicePath << std::endl;
    }
    else {
        std::wcerr << L"Couldn't get executable's path, exiting" << std::endl;
        return 1;
    }

    // console commands
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--install") {
            InstallService(serviceName, servicePath);
            StartUserService(serviceName);
        }
        else if (arg == "--uninstall") {
            UninstallService(serviceName);
        }
        else if (arg == "--update") {
            SendControlCommand(serviceName, SERVICE_CONTROL_UPDATE); 
        }
        else if (arg == "--start") {
            StartUserService(serviceName);
        }
        else if (arg == "--stop") {
            SendControlCommand(serviceName, SERVICE_CONTROL_STOP);
        }
        else if (arg == "--pause") {
            SendControlCommand(serviceName, SERVICE_CONTROL_PAUSE);
        }
        else if (arg == "--continue") {
            SendControlCommand(serviceName, SERVICE_CONTROL_CONTINUE);
        }
        else {
            std::cerr << "Invalid argument. Use --install, --uninstall, --start, --stop, --pause, --continue, or --update.\n";
        }
        std::cout << "Press any key to exit." << std::endl;
        std::cin.get();
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
        while (true)
            Menu(serviceName, servicePath);
    }
    return 0;
}
