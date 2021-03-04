#include "../Common/RunningProcesses.h"

#include <iostream>

#include <TlHelp32.h>
#include <Windows.h>

int main() {
	try
	{
		RunningProcesses processes;
		for (const PROCESSENTRY32& process : processes)
		{
			const std::wstring processName(process.szExeFile);
			const DWORD processPid = process.th32ProcessID;

			std::wcout << " PID=" << processPid << ", Process Name=" << processName << std::endl;
		}
	}
	catch (std::exception& exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}