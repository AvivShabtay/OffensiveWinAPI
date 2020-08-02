#include "..\Helper\common.h"
#include <windows.h>
#include <winternl.h>
#include <tchar.h>

typedef struct _LDR_MODULE {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;

}LDR_MODULE, * PLDR_MODULE;

/*
 * The structure is undocumented and we need to defined it by ourself.
 * http://undocumented.ntinternals.net/index.html?page=UserMode%2FStructures%2FPEB_LDR_DATA.html
 */
typedef struct __PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} Peb_Ldr_Data, * PPeb_Ldr_Data;

DWORD GetPebAddress();
DWORD* GetPebLdrAddress();
DWORD FindModuleBaseAddress(PPeb_Ldr_Data ldrBaseAddress, TCHAR* moduleName);

int main() {
	TCHAR moduleName[] = "DLLExample.dll";
	HANDLE hMyDLL;
	PPEB dwPebAddress;
	PPeb_Ldr_Data ldrAddress;

	// Load external DLL into process memory:
	hMyDLL = LoadLibrary(moduleName);

	// Get the PEB address:
	dwPebAddress = (PPEB)GetPebAddress();

	// Get the Loaded-Modules (PEB_LDR_DATA) list address:
	ldrAddress = (PPeb_Ldr_Data)GetPebLdrAddress();

	// Get the base address of DLL:
	FindModuleBaseAddress(ldrAddress, moduleName);

	return 0;
}
/*
 * Get the current process PEB address.
 * http://www.rohitab.com/discuss/topic/34435-get-base-address-via-process-peb-and-hide-from-peb/
*/
DWORD GetPebAddress() {
	DWORD* dwPebBaseAddress;
	__asm {
		push eax						// Save the current value of EAX
		mov eax, FS: [0x30]				// Get PEB base address
		mov[dwPebBaseAddress], eax		// Return PEB_LDR_DATA address
		pop eax							// Restore EAX value
	}
	return (DWORD)dwPebBaseAddress;
}

DWORD* GetPebLdrAddress() {
	DWORD* dwPebLdrBaseAddress;
	__asm {
		push eax						// Save the current value of EAX
		mov eax, FS: [0x30]				// Get PEB base address
		add eax, 0xC					// Offset to PEB_LDR_DATA address (in PEB)
		mov eax, [eax]					// Get PEB_LDR_DATA base address value
		mov[dwPebLdrBaseAddress], eax	// Return PEB_LDR_DATA address
		pop eax							// Restore EAX value
	}
	return dwPebLdrBaseAddress;
}

DWORD FindModuleBaseAddress(PPeb_Ldr_Data ldrBaseAddress, TCHAR* moduleName) {

	// Get the first _LIST_ENTRY of InMemoryLoadedModules:
	PLDR_MODULE ldrModule = (PLDR_MODULE)ldrBaseAddress->InLoadOrderModuleList.Flink;

	while (ldrModule->BaseAddress != 0) {

		std::wcout << ldrModule->BaseDllName.Buffer << std::endl;

		// Move to the next _LIST_ENTRY:
		ldrModule = (PLDR_MODULE)ldrModule->InLoadOrderModuleList.Flink;

	}
	return 1;
}