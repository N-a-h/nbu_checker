 
#include <iostream>

void InstallService(const wchar_t* serviceName, wchar_t* servicePath);
void UninstallService(const wchar_t* serviceName);
bool IsInstalled(const wchar_t* serviceName);