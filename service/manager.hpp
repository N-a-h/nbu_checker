
#include <iostream>
#include <windows.h>
#include <winsvc.h>

void InstallService(const wchar_t* serviceName, wchar_t* servicePath);
void UninstallService(const wchar_t* serviceName);
bool IsInstalled(const wchar_t* serviceName);
bool SendCommand(const wchar_t* serviceName, DWORD serviceCommand);
