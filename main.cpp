#include "service/service.hpp"
#include "service/installer.hpp"
#include <iostream>
#include <cstdio>
#include <windows.h>

// service name
#define serviceName L"NbuChecker"

// Helper function to convert wstr to str
std::string wstring_to_utf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

// Function to get the directory path of the current executable and append a specific file name
std::string getExecutableDir() {
    wchar_t servicePath[MAX_PATH] = { 0 };
    if (GetModuleFileName(NULL, servicePath, MAX_PATH)) {
        std::wstring path(servicePath);

        // Find the last backslash to isolate the directory
        size_t lastBackslashIndex = path.find_last_of(L'\\');
        if (lastBackslashIndex != std::wstring::npos) {
            // Keep only the directory path
            std::wstring directoryPath = path.substr(0, lastBackslashIndex);
            // Convert the directory path with the appended file name to UTF-8
            return wstring_to_utf8(directoryPath);
        }
        else {
            throw std::runtime_error("Failed to isolate the directory path from the executable path.");
        }
    }
    else {
        throw std::runtime_error("GetModuleFileName failed to retrieve the path.");
    }
}

int main(int argc, char* argv[]) {
    bool loaded = g_Config.loadFromFile("config.json");
    
    if (!loaded) {
        std::cout << "Writing a new default config file" << std::endl;
        nlohmann::json j;
        j["debug"] = true;
        j["interval"] = 20000;
        j["dataFormat"] = "JSON";
        j["dataFilePath"] = getExecutableDir() + "\\data.json";

        std::ofstream cfg("config.json");
        if (cfg.is_open()) {
            cfg << j.dump(4);
        }
        else {
            std::cout << "Can't open config.json, exiting" << std::endl;
            return 1;
        }
        std::cout << "New config has been written, please run the executable again" << std::endl;
        return 0;
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
