#pragma once
#include "AutoVirtualAlloc.h"

#include <vector>
#include <cstddef>

#include <Windows.h>

/*
	48 b8 00 00 00 00 00  00 00 00  movabs rax,0x0
	ff e0							jmp    rax
 */
const BYTE TRAMPOLINE_SHELLCODE[] = "\x48\xb8\x00\x00\x00\x00\x00\x00\x00\x00\xff\xe0";

const BYTE NOP_OPCODE = 0x90;

class TrampolineHook final
{
public:
	explicit TrampolineHook(void* originalFunction, void* hookingFunction, int patchSize);

	~TrampolineHook();

	// Disable: copyable, cloneable, movable:
	TrampolineHook(const TrampolineHook&) = delete;
	TrampolineHook& operator=(const TrampolineHook&) = delete;
	TrampolineHook(const TrampolineHook&&) = delete;
	TrampolineHook& operator=(const TrampolineHook&&) = delete;

	void installHook() const;

	void* getGatewayAddress() const;

private:
	static std::vector<std::byte> getJumpShellcode(void* addressToJumpTo);
	void restoreOriginalBytes() const;
	void createGateway();

	void* m_originalAddress;
	void* m_hookingAddress;
	AutoVirtualAlloc m_gatewayMemory;
	int m_patchSize;
};

