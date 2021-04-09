#include "ProcessUtils.h"
#include "RunningProcesses.h"
#include "Wmi.h"
#include "WmiQuery.h"

#include <stdexcept>

std::uint32_t ProcessUtils::getProcessPidByProcessName(const std::wstring& processName)
{
	RunningProcesses processes;
	for (const auto& process : processes)
	{
		std::wstring currentProcessName(process.szExeFile);

		if (processName == currentProcessName)
		{
			return process.th32ProcessID;
		}
	}

	throw std::runtime_error("Could not find target process PID");
}

WmiProcessIterator ProcessUtils::getAllRunningProcessesFromWmi()
{
	WmiQuery wmiQuery;
	IEnumWbemClassObject* classObjectEnumerator = wmiQuery.executeQuery(ALL_RUNNING_PROCESSES_QUERY);

	WmiProcessIterator processIterator(classObjectEnumerator);
	return processIterator;
}
