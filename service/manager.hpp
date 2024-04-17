 
#include <iostream>

static void InstallService(const wchar_t* serviceName, wchar_t* servicePath);
static void UninstallService(const wchar_t* serviceName);
static bool IsInstalled(const wchar_t* serviceName);
static bool SendCommand(const wchar_t* serviceName, DWORD serviceCommand);