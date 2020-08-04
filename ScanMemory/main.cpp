///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <winternl.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>

#define ERROR_MESSAGE_SIZE 512

// Helper function to test NTSTATUS values:
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

// Prototype to Native-API function:
typedef NTSTATUS(NTAPI* pNtSuspendProcess)(
	IN HANDLE ProcessHandle
	);

// Helper functions:
//BOOL SuspendProc(DWORD dwPid);
VOID GetLastErrorAsString(TCHAR* result);
VOID GetMemoryType(DWORD type, TCHAR* result);
VOID GetMemoryState(DWORD type, TCHAR* result);
VOID GetMemoryProtection(DWORD type, TCHAR* result);

int main(int argc, TCHAR* argv[]) {

	DWORD dwPid;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };
	PVOID* addr = 0;
	HANDLE hProc = nullptr;
	TCHAR fileName[MAX_PATH];
	DWORD dwBytesRead;
	PVOID pBaseAddress = nullptr;
	PVOID pAddressBlock = nullptr;

	// Check input data:
	if (argc < 2) {
		printf("Usage %s <pid>\n", __FILE__);
		return -1;
	}

	// The PID to search:
	dwPid = atoi(argv[1]);

	// Open handle to the process:
	hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hProc == INVALID_HANDLE_VALUE) {
		TCHAR errorMessage[ERROR_MESSAGE_SIZE] = { 0 };
		GetLastErrorAsString(errorMessage);

		printf("[-] Could not open handle to process PID=%d, Error: %s\n", dwPid, errorMessage);
		return -1;
	}

	// Query memory information of another process:
	dwBytesRead = VirtualQueryEx(hProc, &addr, &memoryInfo, sizeof(memoryInfo));
	if (dwBytesRead == 0) {
		TCHAR errorMessage[ERROR_MESSAGE_SIZE] = { 0 };
		GetLastErrorAsString(errorMessage);

		printf("[-] Could read memory using VirtualQueryEx, Error: %s\n", errorMessage);

		CloseHandle(hProc);
		return -1;
	}

	// Setup pointers for iteration:
	pBaseAddress = memoryInfo.AllocationBase;
	pAddressBlock = pBaseAddress;

	printf("[+] Print the process memory regions with mapped file:\n");

	// Find all the mapped modules used by the process:
	while (TRUE) {

		// Get memory info about the current block of memory:
		dwBytesRead = VirtualQueryEx(hProc, pAddressBlock, &memoryInfo, sizeof(memoryInfo));
		if (dwBytesRead != sizeof(memoryInfo)) {
			break;
		}

		// Try to get mapped file name to the memory range:
		DWORD success = GetMappedFileName(hProc, memoryInfo.BaseAddress, fileName, MAX_PATH);

		// Successfully get mapped file to the memory range:
		if (success) {
			TCHAR memoryType[10] = { 0 };
			TCHAR memoryState[10] = { 0 };
			TCHAR memoryProtection[10] = { 0 };

			GetMemoryType(memoryInfo.Type, memoryType);
			GetMemoryState(memoryInfo.State, memoryState);
			GetMemoryProtection(memoryInfo.Protect, memoryProtection);

			printf("[*] Address: 0x%08p | Base: 0x%08p | Type: %-6s | State: %-7s | Protection: %-3s | FileName: %s\n",
				pAddressBlock, memoryInfo.BaseAddress, memoryType, memoryState, memoryProtection, fileName);
		}

		// Move to the next memory range:
		pAddressBlock = (PVOID)((PBYTE)pAddressBlock + memoryInfo.RegionSize);
	}

	// Release resources:
	CloseHandle(hProc);
}

/*
 * Convert the last error code into readable message and copy
 * the message into the given buffer.
*/
VOID GetLastErrorAsString(TCHAR* result) {
	DWORD dwErrorCode;
	DWORD dwSize;
	TCHAR message[ERROR_MESSAGE_SIZE];

	dwErrorCode = GetLastError();

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErrorCode,
		0, message, ERROR_MESSAGE_SIZE, NULL);

	CopyMemory(result, message, ERROR_MESSAGE_SIZE);
}

/*
 * Convert memory type mask into common name.
*/
VOID GetMemoryType(DWORD type, TCHAR* result) {
	if (type & MEM_IMAGE)
		CopyMemory(result, "Image", 5);

	else if (type & MEM_MAPPED)
		CopyMemory(result, "Mapped", 6);

	else if (type & MEM_PRIVATE)
		CopyMemory(result, "Private", 7);
	else
		return;
}

/*
 * Convert memory state mask into common name.
*/
VOID GetMemoryState(DWORD type, TCHAR* result) {
	if (type & MEM_COMMIT)
		CopyMemory(result, "Commit", 6);

	else if (type & MEM_FREE)
		CopyMemory(result, "Free", 4);

	else if (type & MEM_RESERVE)
		CopyMemory(result, "Reserve", 7);
	else
		return;
}

/*
 * Convert memory protection mask into common name.
*/
VOID GetMemoryProtection(DWORD type, TCHAR* result) {
	if (type & PAGE_READONLY)
		CopyMemory(result, "R", 1);

	else if (type & PAGE_WRITECOPY)
		CopyMemory(result, "WC", 2);

	else if (type & PAGE_READWRITE)
		CopyMemory(result, "RW", 2);

	else if (type & PAGE_NOACCESS)
		CopyMemory(result, "NA", 2);

	else if (type & PAGE_EXECUTE)
		CopyMemory(result, "X", 1);

	else if (type & PAGE_EXECUTE_READ)
		CopyMemory(result, "RX", 2);

	else if (type & PAGE_EXECUTE_READWRITE)
		CopyMemory(result, "RWX", 3);

	else if (type & PAGE_EXECUTE_WRITECOPY)
		CopyMemory(result, "WCX", 3);

	else
		return;
}