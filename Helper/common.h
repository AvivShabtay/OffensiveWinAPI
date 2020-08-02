#pragma once
#include "pch.h"
#include "framework.h"
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <utility>
#include <strsafe.h>
#include <iostream>

class Common {
public:
	static BOOL GetAllRunningProcesses(std::vector<std::pair<DWORD, WCHAR[260]>>& procList);
	static DWORD FindPidByProcessName(WCHAR* processName);
};


