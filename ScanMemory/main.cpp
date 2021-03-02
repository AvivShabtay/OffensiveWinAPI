//////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020-2021, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//////////////////////////////////////////////////////////////////////

#include "../Common/AutoHandle.h"
#include "../Common/Win32ErrorCodeException.h"
#include "MemoryInformation.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <atlstr.h>

#include <Psapi.h>
#include <Windows.h>


/* Convert memory type mask into common name. */
std::wstring GetMemoryType(DWORD type);

/* Convert memory state mask into common name. */
std::wstring GetMemoryState(DWORD type);

/* Convert memory protection mask into common name. */
std::wstring GetMemoryProtection(DWORD type);

int wmain(int argc, PWCHAR argv[])
{

	if (argc < 2)
	{
		std::cout << "Usage ScanMemory.exe <pid>" << std::endl;
		std::cout << "\tpid - PID of the process to be scanned" << std::endl;
		return 1;
	}

	const DWORD processPid = _wtoi(argv[1]);

	try
	{
		const AutoHandle process(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processPid));
		if (INVALID_HANDLE_VALUE == process.get())
		{
			throw Win32ErrorCodeException("Could not open handle to process");
		}

		MemoryInformation memoryInformation(processPid);
		for (const MEMORY_BASIC_INFORMATION& memory : memoryInformation)
		{
			std::vector<WCHAR> filenameData(MAX_PATH);
			std::wstring filename(L"No mapped file");

			if (GetMappedFileName(process.get(), memory.BaseAddress, &filenameData[0], MAX_PATH))
			{
				filename = static_cast<WCHAR*>(filenameData.data());
			}

			std::wstring type = GetMemoryType(memory.Type);
			std::wstring state = GetMemoryState(memory.State);
			std::wstring protection = GetMemoryProtection(memory.Protect);

			printf("[*] Base: 0x%08p | Type: %-6ws | State: %-5ws | Protection: %-3ws | FileName: %ws\n",
				memory.BaseAddress, type.c_str(), state.c_str(), protection.c_str(), filename.c_str());
		}
	}
	catch (std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}

std::wstring GetMemoryType(DWORD type)
{
	if (type & MEM_COMMIT)
	{
		return L"Commit";
	}

	if (type & MEM_RESERVE)
	{
		return L"Reserve";
	}

	if (type & MEM_IMAGE)
	{
		return L"Image";
	}

	else if (type & MEM_MAPPED)
	{
		return L"Mapped";
	}

	else if (type & MEM_PRIVATE)
	{
		return L"Private";
	}

	else if (type & MEM_FREE)
	{
		return L"Free";
	}

	else if (type == 0)
	{
		return L"Unusable"; // Process Hacker
	}

	else
	{
		return L"";
	}
}

std::wstring GetMemoryState(DWORD type)
{
	if (type & MEM_COMMIT)
	{
		return L"Commit";
	}

	else if (type & MEM_FREE)
	{
		return L"Free";
	}

	else if (type & MEM_RESERVE)
	{
		return L"Reserve";
	}

	else
	{
		return L"";
	}
}

std::wstring GetMemoryProtection(DWORD type)
{
	if (type & PAGE_READONLY)
	{
		return L"R";
	}

	else if (type & PAGE_WRITECOPY)
	{
		return L"WC";
	}

	else if (type & PAGE_READWRITE)
	{
		return L"RW";
	}

	else if (type & PAGE_NOACCESS)
	{
		return L"NA";
	}

	else if (type & PAGE_EXECUTE)
	{
		return L"X";
	}

	else if (type & PAGE_EXECUTE_READ)
	{
		return L"RX";
	}

	else if (type & PAGE_EXECUTE_READWRITE)
	{
		return L"RWX";
	}

	else if (type & PAGE_EXECUTE_WRITECOPY)
	{
		return L"WCX";
	}

	else
	{
		return L"";
	}
}