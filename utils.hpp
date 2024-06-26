#include <string>
#include <codecvt>
#include <cstdint>
#include <string>
#include <windows.h>


/// <summary>
/// Converts std::wstring to std::string
/// </summary>
/// <param name="wstr">wstring to convert</param>
/// <returns>converted std::string</returns>
static std::string wstring_to_utf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

/// <summary>
/// Gets the executable's path
/// </summary>
/// <returns>path</returns>
static std::string getExecutableDir() {
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
