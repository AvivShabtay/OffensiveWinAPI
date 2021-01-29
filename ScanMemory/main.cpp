//////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020-2021, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <winternl.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <atlstr.h>

// Helper function to test NTSTATUS values:
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

using pNtSuspendProcess = NTSTATUS(NTAPI*)(IN HANDLE);
//BOOL SuspendProc(DWORD dwPid);

// Helper functions:
std::wstring GetLastErrorMessage();
std::wstring GetMemoryType(DWORD type);
std::wstring GetMemoryState(DWORD type);
std::wstring GetMemoryProtection(DWORD type);

int main(int argc, TCHAR* argv[]) {

	// Check input data:
	if (argc < 2) {
		printf("Usage %s <pid>\n", __FILE__);
		return -1;
	}

	// The PID to search:
	DWORD dwPid = atoi(argv[1]);

	// Open handle to the process:
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hProc == INVALID_HANDLE_VALUE) {
		auto errorMessage = GetLastErrorMessage();
		printf("[-] Could not open handle to process PID=%d, Error: %ws\n", dwPid, errorMessage.c_str());
		return -1;
	}

	// Query memory information of another process:
	LPVOID lpAddress = nullptr;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };
	if (!VirtualQueryEx(hProc, lpAddress, &memoryInfo, sizeof(memoryInfo))) {
		auto errorMessage = GetLastErrorMessage();
		printf("[-] Could read memory using VirtualQueryEx, Error: %ws\n", errorMessage.c_str());

		CloseHandle(hProc);
		return -1;
	}

	// Setup pointers for iteration:
	PVOID pBaseAddress = memoryInfo.AllocationBase;
	PVOID pAddressBlock = pBaseAddress;

	printf("[+] Print the process memory regions with mapped file:\n");

	// Find all the mapped modules used by the process:
	while (TRUE) {

		// Get memory info about the current block of memory
		if (VirtualQueryEx(hProc, pAddressBlock, &memoryInfo, sizeof(memoryInfo)) != sizeof(memoryInfo)) {
			break;
		}

		// Try to get mapped file name to the memory range:
		TCHAR fileName[MAX_PATH];
		if (GetMappedFileName(hProc, memoryInfo.BaseAddress, fileName, MAX_PATH)) {

			auto type = GetMemoryType(memoryInfo.Type);
			auto state = GetMemoryState(memoryInfo.State);
			auto protection = GetMemoryProtection(memoryInfo.Protect);

			printf("[*] Address: 0x%08p | Base: 0x%08p | Type: %-6ws | State: %-7ws | Protection: %-3ws | FileName: %s\n",
				pAddressBlock, memoryInfo.BaseAddress, type.c_str(), state.c_str(), protection.c_str(), fileName);
		}

		// Move to the next memory range:
		pAddressBlock = (PVOID)((PBYTE)pAddressBlock + memoryInfo.RegionSize);
	}

	CloseHandle(hProc);
}

/* Convert the last error code into readable message and copy
   the message into the given buffer. */
std::wstring GetLastErrorMessage() {

	DWORD dwErrorCode = GetLastError();
	if (dwErrorCode == 0) {
		return std::wstring();
	}

	const int MESSAGE_SIZE = 512;
	CHAR message[MESSAGE_SIZE];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErrorCode,
		0, message, MESSAGE_SIZE, NULL);

	return std::wstring(CStringW(const_cast<CHAR*>(message)).GetString());
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