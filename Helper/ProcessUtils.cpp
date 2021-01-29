#include "pch.h"
#include "ProcessUtils.h"

DWORD ProcessUtils::FindPidByProcessName(std::wstring processName) {

	std::map<DWORD, std::wstring> procList;
	ProcessUtils::GetAllRunningProcesses(procList);

	for (auto& proc : procList) {
		if (wcscmp(processName.c_str(), proc.second.c_str()) == 0) {
			return proc.first;
		}
	}
	return -1;
}


void ProcessUtils::GetAllRunningProcesses(std::map<DWORD, std::wstring>& procList) {

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	// Get the first running process:
	Process32First(hSnapshot, &processEntry);
	do {
		auto pid = processEntry.th32ProcessID;
		auto procName = std::wstring(const_cast<WCHAR*>(processEntry.szExeFile));

		procList.insert({ pid, procName });

	} while (Process32Next(hSnapshot, &processEntry));

	CloseHandle(hSnapshot);
}

