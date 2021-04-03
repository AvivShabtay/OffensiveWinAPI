#include "TrampolineHook.h"
#include "VirtualProtectGuard.h"

#include <iostream>

#include <Windows.h>

TrampolineHook::TrampolineHook(void* originalFunction, void* hookingFunction, int patchSize)
	: m_originalAddress(originalFunction), m_hookingAddress(hookingFunction), m_gatewayMemory(), m_patchSize(patchSize)
{
	if (nullptr == this->m_originalAddress)
	{
		throw std::runtime_error("Invalid original function address");
	}

	if (nullptr == this->m_hookingAddress)
	{
		throw std::runtime_error("Invalid hooking function address");
	}

	if (patchSize < sizeof(TRAMPOLINE_SHELLCODE))
	{
		throw std::runtime_error("Invalid patch size");
	}

	this->createGateway();
}

TrampolineHook::~TrampolineHook()
{
	try
	{
		this->restoreOriginalBytes();
	}
	catch (...)
	{
		std::cout << "Exception thrown in destructor of: IatHook" << std::endl;
	}
}

std::vector<std::byte> TrampolineHook::getJumpShellcode(void* addressToJumpTo)
{
	std::vector<std::byte> jumpShellcode;
	for (const BYTE opcode : TRAMPOLINE_SHELLCODE)
	{
		jumpShellcode.push_back(static_cast<std::byte>(opcode));
	}

	// Write the address "to jump to":
	memcpy_s(&jumpShellcode[2], sizeof(void*), addressToJumpTo, sizeof(void*));

	return jumpShellcode;
}

void TrampolineHook::restoreOriginalBytes() const
{
	VirtualProtectGuard virtualProtectGuard(this->m_originalAddress, this->m_patchSize, PAGE_EXECUTE_READWRITE);

	memcpy_s(this->m_originalAddress, this->m_patchSize, this->m_gatewayMemory.get(), this->m_patchSize);
}

void TrampolineHook::createGateway()
{
	// Original function + jump shellcode:
	const int gatewaySize = this->m_patchSize + sizeof(TRAMPOLINE_SHELLCODE);

	this->m_gatewayMemory.reset(gatewaySize, MEM_RELEASE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// Copy to original bytes:
	memcpy_s(this->m_gatewayMemory.get(), this->m_patchSize, this->m_originalAddress, this->m_patchSize);

	// Create jump shellcode to the original function body (after the hook):
	std::vector<std::byte> jumpShellcode = this->getJumpShellcode(static_cast<PBYTE>(this->m_originalAddress) + this->m_patchSize);

	// Insert jump to the body of the original address:
	memcpy_s(static_cast<PBYTE>(this->m_gatewayMemory.get()) + this->m_patchSize, sizeof(TRAMPOLINE_SHELLCODE),
		jumpShellcode.data(), sizeof(TRAMPOLINE_SHELLCODE));
}

void TrampolineHook::installHook() const
{
	std::vector<std::byte> jumpShellcode = this->getJumpShellcode(this->m_hookingAddress);

	if (jumpShellcode.size() < this->m_patchSize)
	{
		for (int i = jumpShellcode.size(); i < this->m_patchSize; i++)
		{
			// Fill the rest of the patch with NOPs:
			jumpShellcode.push_back(static_cast<std::byte>(NOP_OPCODE));
		}
	}

	VirtualProtectGuard virtualProtectGuard(this->m_originalAddress, this->m_patchSize, PAGE_EXECUTE_READWRITE);

	memcpy_s(this->m_originalAddress, this->m_patchSize, jumpShellcode.data(), this->m_patchSize);
}

void* TrampolineHook::getGatewayAddress() const
{
	return this->m_gatewayMemory.get();
}
