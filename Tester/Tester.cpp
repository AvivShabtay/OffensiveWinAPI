#include "..\Helper\common.h"
#include <iostream>

int main() {
	std::vector<std::pair<DWORD, WCHAR[260]>> procList;
	WCHAR procName[] = L"csrss.exe";
	DWORD pid;

	BOOL result = Common::GetAllRunningProcesses(procList);
	if (result) {
		for (DWORD i = 0; i < procList.size(); i++) {
			DWORD procID = procList.at(i).first;
			std::wstring procName(procList.at(i).second);
			std::wcout << procID << ", " << procName << std::endl;
		}
	}

	pid = Common::FindPidByProcessName(procName);
	std::wcout << "PID of " << procName << " is: " << pid << std::endl;

	return 0;
}