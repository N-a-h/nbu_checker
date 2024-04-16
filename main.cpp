#include "service.hpp"

int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {(LPWSTR)L"CurrencyService", CurrencyService::ServiceMain},
        {NULL, NULL}
    };

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
        return GetLastError();
    }

    return 0;
}
