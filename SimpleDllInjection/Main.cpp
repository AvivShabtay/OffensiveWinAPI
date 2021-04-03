#include "../Common/DebugPrint.h"
#include "../Common/AutoHandle.h"
#include "../Common/Win32ErrorCodeException.h"
#include "../Common/VirtualAllocExGuard.h"

#include <exception>
#include <iostream>

#include <Windows.h>

void printUsage();

int wmain(int argc, PWCHAR argv[])
{
	if (2 > argc)
	{
		printUsage();
		return 1;
	}

	try
	{
		const std::uint32_t targetPid = _wtoi(argv[1]);
		const std::wstring injectedDllPath = argv[2];

		const AutoHandle targetProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPid));
		if (nullptr == targetProcess.get())
		{
			throw Win32ErrorCodeException("Could not open handle to target process");
		}


		VirtualAllocExGuard targetMemory(targetProcess.get(), injectedDllPath.size() * sizeof(WCHAR),
			PAGE_READWRITE, MEM_COMMIT);
		if (nullptr == targetMemory.get())
		{
			throw Win32ErrorCodeException("Could not allocate memory in target process");
		}

		SIZE_T bytesWritten;
		if (!WriteProcessMemory(targetProcess.get(), targetMemory.get(), injectedDllPath.data(), injectedDllPath.size() * sizeof(WCHAR), &bytesWritten))
		{
			throw Win32ErrorCodeException("Could not write injected DLL path into target process memory");
		}


		DWORD threadId;
		const AutoHandle targetThread(CreateRemoteThread(targetProcess.get(), nullptr, 0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), targetMemory.get(), 0, &threadId));

		if (nullptr == targetThread.get())
		{
			throw Win32ErrorCodeException("Could not create remote thread in target process");
		}

		WaitForSingleObject(targetThread.get(), INFINITE);

		// Injection succeeded:
		targetMemory.release();
	}
	catch (std::exception& exception)
	{
		DEBUG_PRINT(exception.what());
		return 1;
	}
	catch (...)
	{
		DEBUG_PRINT("Exception was thrown");
		return 1;
	}

	return 0;
}

void printUsage()
{
	std::cout
		<< "Usage SimpleDllInjection.exe <pid> <DLL path>"
		<< "\npid - Target process PID"
		<< "\nDLL path - Injected DLL path"
		<< std::endl;
}