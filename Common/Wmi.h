#pragma once

#include <string>

// WMI library file:
#pragma comment(lib, "wbemuuid.lib")
const std::wstring WMI_NAMESPACE(L"ROOT\\CIMV2");
const std::wstring WMI_QUERY_LANGUAGE(L"WQL");
const std::wstring ALL_RUNNING_PROCESSES_QUERY(L"SELECT * FROM Win32_Process");