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

#define BYTES_TO_READ 4

DWORD PasswordToInteger(TCHAR* pws);
BOOL XorEncryptDecrypt(LPTSTR pSource, LPTSTR pDestination, DWORD password);

int main(int argc, TCHAR* argv[]) {


	if (argc < 3) {
		printf("Usage: <source> <destination> <password>\n");
		return -1;
	}

	LPTSTR pSourceFile = argv[1];
	LPTSTR pDestinationFile = argv[2];
	LPTSTR pPassword = argv[3];

	if (XorEncryptDecrypt(pSourceFile, pDestinationFile, PasswordToInteger(pPassword))) {
		printf("Encryption of the file %s was successful. \n", pSourceFile);
		printf("The encrypted data is in file %s.\n", pDestinationFile);
		return 0;
	}
	else {
		printf("[-] Could not encrypt / decrypt the file.\n");
		return -1;
	}

	return 0;
}

/*
 * Use XOR operation to transfer the data of given file for Encryption
 * or Decryption operation with a given password key.
*/
BOOL XorEncryptDecrypt(LPTSTR pSource, LPTSTR pDestination, DWORD password) {
	HANDLE hSource = nullptr;
	HANDLE hDestination = nullptr;
	char buffer[BYTES_TO_READ] = { 0 };
	DWORD dwBytesRead;
	DWORD dwBytesWrite;
	BOOL endOfFile = FALSE;

	hSource = CreateFile(pSource, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSource == INVALID_HANDLE_VALUE) {
		printf("[-] Could not open handle to source file: %s, Error:0x%08x\n", pSource, GetLastError());
		return FALSE;
	}

	hDestination = CreateFile(pDestination, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDestination == INVALID_HANDLE_VALUE) {
		printf("[-] Could not open handle to destination file: %s, Error:0x%08x\n", pSource, GetLastError());
		CloseHandle(hSource);
		return FALSE;
	}

	do {
		// Check for successful reading:
		if (!ReadFile(hSource, buffer, BYTES_TO_READ, &dwBytesRead, NULL)) {
			printf("[-] Error while reading from source file, Error:0x%08x\n", GetLastError());
			CloseHandle(hSource);
			CloseHandle(hDestination);
			return FALSE;
		}

		// Indicates EOF:
		if (dwBytesRead < BYTES_TO_READ) {
			endOfFile = TRUE;
		}

		// Encrypt/Decrypt:
		*buffer = ((DWORD)*buffer) ^ ((DWORD)password);

		// Check for successful writing:
		if (!WriteFile(hDestination, buffer, dwBytesRead, &dwBytesWrite, NULL)) {
			printf("[-] Error while writing to destination file, Error:0x%08x\n", GetLastError());
			CloseHandle(hSource);
			CloseHandle(hDestination);
			return FALSE;
		}

	} while (!endOfFile);

	CloseHandle(hSource);
	CloseHandle(hDestination);
	return TRUE;
}

/*
 * Convert given String into DWORD for the XOR operation.
*/
DWORD PasswordToInteger(TCHAR* pws) {
	int x = 0;
	int i = 0;

	// Initial value:
	x = 1586;

	for (i = 0; i < _tcslen(pws); i++) {
		x += (int)pws[i];
	}
	return x;
}