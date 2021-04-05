#pragma once

#include <Windows.h>

struct PicParams
{
	LPVOID loadLibraryA;
	LPVOID getProcAddress;
};

typedef HMODULE(WINAPI* pLoadLibraryA)(_In_ LPCSTR lpLibFileName);
typedef FARPROC(WINAPI* pGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
typedef int(WINAPI* pMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType);

#ifdef __cplusplus
extern "C" {
#endif

	DWORD WINAPI startPic(struct PicParams* params);

	void endPic();

#ifdef __cplusplus
}
#endif