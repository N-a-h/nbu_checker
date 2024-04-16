#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>

class Logger {
private:
    bool debug;
    const wchar_t* serviceName;

    void writeToLogFile(const std::wstring& message) {
        std::wofstream logFile("log.txt", std::ios_base::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            logFile.close();
        }
        else {
            std::wcerr << L"Error: Could not open log file for writing." << std::endl;
        }
    }

    void writeToEventViewer(const std::wstring& message, WORD eventType) {
        HANDLE hEventSource = RegisterEventSource(NULL, serviceName);
        if (hEventSource) {
            LPCWSTR lpszStrings[1];
            lpszStrings[0] = message.c_str();
            ReportEvent(hEventSource, eventType, 0, 0, NULL, 1, 0, lpszStrings, NULL);
            DeregisterEventSource(hEventSource);
        }
        else {
            std::cerr << "Error: Could not register event source." << std::endl;
        }
    }



public:
    Logger(bool debugMode) : debug(debugMode) {}

    void Error(const std::wstring& message) {
        if (debug) {
            writeToLogFile(L"[ERROR] " + message);
        }
        else {
            writeToEventViewer(message, EVENTLOG_ERROR_TYPE);
        }
    }

    void Warn(const std::wstring& message) {
        if (debug) {
            writeToLogFile(L"[WARN] " + message);
        }
        else {
            writeToEventViewer(message, EVENTLOG_WARNING_TYPE);
        }
    }

    void Info(const std::wstring& message) {
        if (debug) {
            writeToLogFile(L"[INFO] " + message);
        }
        else {
            writeToEventViewer(message, EVENTLOG_INFORMATION_TYPE);
        }
    }
};