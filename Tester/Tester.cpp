#include <Windows.h>
#include <iostream>
#include <map>
#include "..\Helper\ProcessUtils.h"

int main() {
	std::map<DWORD, std::wstring> procList;

	ProcessUtils::GetAllRunningProcesses(procList);
	if (0 < procList.size()) {
		for (auto& [pid, procName] : procList) {
			std::wcout << pid << ", " << procName << std::endl;
		}
	}

	std::wstring procName = L"csrss.exe";
	DWORD pid = ProcessUtils::FindPidByProcessName(procName);
	std::wcout << "PID of " << procName << " is: " << pid << std::endl;

	return 0;
}