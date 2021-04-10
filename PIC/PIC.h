#pragma once

#include <Windows.h>

struct PicParams
{
	LPVOID loadLibraryA;
	LPVOID getProcAddress;
	SIZE_T picSize;
};

struct CleanupRop
{
	DWORD_PTR ExitThreadAddress;	// lpAddress
	LPVOID lpAddress;				// VirtualFree address of region to be freed
	SIZE_T dwSize;					// VirtualFree size of the region to free
	DWORD dwFreeType;				// VirtualFree type of allocation to free (MEM_RELEASE)
	DWORD dwExitCode;				// ExitThread thread exit code value
};

typedef HMODULE(WINAPI* pLoadLibraryA)(_In_ LPCSTR lpLibFileName);
typedef FARPROC(WINAPI* pGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
typedef int(WINAPI* pMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType);
typedef BOOL(WINAPI* pVirtualFree)(_Pre_notnull_ _When_(dwFreeType == MEM_DECOMMIT, _Post_invalid_)
	_When_(dwFreeType == MEM_RELEASE, _Post_ptr_invalid_) LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD dwFreeType);
typedef VOID(WINAPI* pExitThread)(_In_ DWORD dwExitCode);
typedef BOOL(WINAPI* pSetThreadContext)(_In_ HANDLE hThread, _In_ CONST CONTEXT* lpContext);
typedef HANDLE(WINAPI* pGetCurrentThread)(VOID);

#ifdef __cplusplus
extern "C" {
#endif

	DWORD WINAPI startPic(struct PicParams* params);

	void endPic();

#ifdef __cplusplus
}
#endif