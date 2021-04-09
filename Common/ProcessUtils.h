#pragma once

#include <cstdint>
#include <string>

#include "WmiProcessIterator.h"

class ProcessUtils final
{
public:
	static std::uint32_t getProcessPidByProcessName(const std::wstring& processName);

	/*
	 * @note: DO NOT forget to create ComInitializationGuard in the relevant context before using this function.
	 * @usage:
			ComInitializationGuard comInitializationGuard;
			{
				WmiProcessIterator processIterator = ProcessUtils::getAllRunningProcessesFromWmi();
				// do something...
			}
	 */
	static WmiProcessIterator getAllRunningProcessesFromWmi();
};

