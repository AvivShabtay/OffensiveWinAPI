#pragma once
//#include "pch.h"

class ProcessUtils {
public:
	/* Get map contains PID and process name of all currently running processes. */
	static void GetAllRunningProcesses(std::map<DWORD, std::wstring>& procList);

	/* Iterate over all running processes and search for process PID by it's process name. */
	static DWORD FindPidByProcessName(std::wstring processName);
};


