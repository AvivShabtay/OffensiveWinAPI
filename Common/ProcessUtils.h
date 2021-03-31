#pragma once

#include <cstdint>
#include <string>

class ProcessUtils final
{
public:
	static std::uint32_t getProcessPidByProcessName(const std::wstring& processName);
};

