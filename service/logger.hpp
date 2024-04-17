#include <iostream>
#include <fstream>
#include <string>
#include "config.hpp"

/// <summary>
/// Used for easier logging(if debug = true the logs will be written to a file rather than event viewer)
/// </summary>
class Logger {
private:
    bool debug;

    /// <summary>
    /// Writes to the log file
    /// </summary>
    /// <param name="message">msg to write</param>
    void writeToLogFile(const std::wstring& message) {
        std::wofstream logFile(getExecutableDir() + "\\log.txt", std::ios_base::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            logFile.close();
        }
        else {
            std::wcerr << L"Error: Could not open log file for writing." << std::endl;
        }
    }

    /// <summary>
    /// Writes to the event log
    /// </summary>
    /// <param name="message">msg to write</param>
    /// <param name="eventType">event type</param>
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

extern Logger g_Logger;