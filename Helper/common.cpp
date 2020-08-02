#include "pch.h"
#include "framework.h"
#include "common.h"

DWORD Common::FindPidByProcessName(WCHAR* processName) {

	std::vector<std::pair<DWORD, WCHAR[260]>> procList;
	Common::GetAllRunningProcesses(procList);

	for (auto& proc : procList) {
		if (wcscmp(processName, proc.second) == 0) {
			return proc.first;
		}
	}
	return -1;
}


BOOL Common::GetAllRunningProcesses(std::vector<std::pair<DWORD, WCHAR[260]>>& procList) {

	//std::vector<std::pair<DWORD, LPTSTR>> procList();
	PROCESSENTRY32 processEntry;
	HANDLE hSnapshot;
	TCHAR tempProcName[260];

	// Get the list of all running processes:
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Initialize the size of the structure:
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	// Get the first running process:
	Process32First(hSnapshot, &processEntry);

	do {

		std::pair<DWORD, WCHAR[260]> element;
		std::wstring procName(processEntry.szExeFile);

		element.first = processEntry.th32ProcessID;
		wcscpy_s(element.second, 260, processEntry.szExeFile);

		procList.push_back(element);

	} while (Process32Next(hSnapshot, &processEntry));

	CloseHandle(hSnapshot);
	return TRUE;

	//return procList;
}

