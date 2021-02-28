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

#include <iostream>
#include <stdexcept>

#include <atlstr.h>

#include <Psapi.h>
#include <Windows.h>

// Helper function to test NTSTATUS values:
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

using pNtSuspendProcess = NTSTATUS(NTAPI*)(IN HANDLE);
//BOOL SuspendProc(DWORD dwPid);

// Helper functions:
//std::string GetLastErrorMessage();
std::wstring GetMemoryType(DWORD type);
std::wstring GetMemoryState(DWORD type);
std::wstring GetMemoryProtection(DWORD type);

int wmain(int argc, PWCHAR argv[]) {

	// Check input data:
	if (argc < 2) {
		printf("Usage %s <pid>\n", __FILE__);
		return -1;
	}

	const DWORD processPid = _wtoi(argv[1]);

	try
	{
		const AutoHandle process(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processPid));
		if (INVALID_HANDLE_VALUE == process.get())
		{
			throw Win32ErrorCodeException("Could not open handle to process");
		}

		// Query memory information of another process:
		LPVOID lpAddress = nullptr;
		MEMORY_BASIC_INFORMATION memoryInfo = { 0 };
		if (0 == VirtualQueryEx(process.get(), lpAddress, &memoryInfo, sizeof(memoryInfo)))
		{
			throw Win32ErrorCodeException("Could read process virtual memory");
		}

		// Setup pointers for iteration:
		PVOID pBaseAddress = memoryInfo.AllocationBase;
		PVOID pAddressBlock = pBaseAddress;

		printf("[+] Print the process memory regions with mapped file:\n");

		// Find all the mapped modules used by the process:
		while (TRUE) {

			// Get memory info about the current block of memory
			if (VirtualQueryEx(process.get(), pAddressBlock, &memoryInfo, sizeof(memoryInfo)) != sizeof(memoryInfo)) {
				break;
			}

			// Try to get mapped file name to the memory range:
			WCHAR fileName[MAX_PATH];
			if (GetMappedFileName(process.get(), memoryInfo.BaseAddress, fileName, MAX_PATH)) {

				auto type = GetMemoryType(memoryInfo.Type);
				auto state = GetMemoryState(memoryInfo.State);
				auto protection = GetMemoryProtection(memoryInfo.Protect);

				printf("[*] Address: 0x%08p | Base: 0x%08p | Type: %-6ws | State: %-7ws | Protection: %-3ws | FileName: %ws\n",
					pAddressBlock, memoryInfo.BaseAddress, type.c_str(), state.c_str(), protection.c_str(), fileName);
			}

			// Move to the next memory range:
			pAddressBlock = (PVOID)((PBYTE)pAddressBlock + memoryInfo.RegionSize);
		}
	}
	catch (std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}

/* Convert memory type mask into common name. */
std::wstring GetMemoryType(DWORD type) {
	if (type & MEM_IMAGE)
		return L"Image";

	else if (type & MEM_MAPPED)
		return L"Mapped";

	else if (type & MEM_PRIVATE)
		return L"Private";
	else
		return L"";
}

/* Convert memory state mask into common name. */
std::wstring GetMemoryState(DWORD type) {
	if (type & MEM_COMMIT)
		return L"Commit";

	else if (type & MEM_FREE)
		return L"Free";

	else if (type & MEM_RESERVE)
		return L"Reserve";
	else
		return L"";
}

/* Convert memory protection mask into common name. */
std::wstring GetMemoryProtection(DWORD type) {

	if (type & PAGE_READONLY)
		return L"R";

	else if (type & PAGE_WRITECOPY)
		return L"WC";

	else if (type & PAGE_READWRITE)
		return L"RW";

	else if (type & PAGE_NOACCESS)
		return L"NA";

	else if (type & PAGE_EXECUTE)
		return L"X";

	else if (type & PAGE_EXECUTE_READ)
		return L"RX";

	else if (type & PAGE_EXECUTE_READWRITE)
		return L"RWX";

	else if (type & PAGE_EXECUTE_WRITECOPY)
		return L"WCX";

	else
		return L"";
}