#include "../Common/RunningProcesses.h"
#include "../Common/WmiProcessIterator.h"
#include "../Common/ComInitializationGuard.h"
#include "../Common/ProcessUtils.h"

#include <iostream>

#include <TlHelp32.h>
#include <Windows.h>

void printProcessesUsingSnapshot();
void printProcessesUsingWmi();

int main() {
	try
	{
		//printProcessesUsingSnapshot();
		printProcessesUsingWmi();
	}
	catch (std::exception& exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}

void printProcessesUsingSnapshot()
{
	RunningProcesses processes;
	for (const PROCESSENTRY32& process : processes)
	{
		const std::wstring processName(process.szExeFile);
		const DWORD processPid = process.th32ProcessID;

		std::wcout << " PID=" << processPid << ", Process Name=" << processName << std::endl;
	}
}

void printProcessesUsingWmi()
{
	ComInitializationGuard comInitializationGuard;

	{
		WmiProcessIterator processIterator = ProcessUtils::getAllRunningProcessesFromWmi();
		for (const auto& process : processIterator)
		{
			std::wcout << "PID= " << process.processId << ", Name= " << process.processName << std::endl;
		}
	}
}
