
#include <iostream>
#include <windows.h>
#include <winsvc.h>

void InstallService(const wchar_t* serviceName, wchar_t* servicePath);
bool StartUserService(const wchar_t* serviceName);
void UninstallService(const wchar_t* serviceName);
bool IsInstalled(const wchar_t* serviceName);

bool SendCustomCommand(const wchar_t* serviceName, DWORD serviceCommand);
bool SendControlCommand(const wchar_t* serviceName, DWORD controlCommand);