#include "PIC.h"

#pragma code_seg(".text$AAAA")
DWORD WINAPI startPic(struct PicParams* params)
{
	//__debugbreak();
	pLoadLibraryA loadLibraryA = (pLoadLibraryA)(params->loadLibraryA);
	pGetProcAddress getProcAddress = (pGetProcAddress)params->getProcAddress;

	CHAR user32Dll[] = { 'u','s','e','r','3','2','.','d','l','l','\0' };
	CHAR messageBoxAName[] = { 'M','e','s','s','a','g','e','B','o','x','A','\0' };
	CHAR message[] = { 'H','e','l','l','o',' ','F','r','o','m',' ','P','I','C',' ','!','\0' };

	HMODULE user32Module = loadLibraryA(user32Dll);
	pMessageBoxA messageBoxA = (pMessageBoxA)getProcAddress(user32Module, messageBoxAName);

	messageBoxA(NULL, message, message, MB_OK);

	return 0;
}

#pragma code_seg(".text$AAAB")
void endPic()
{
	// Left blank internationally
}