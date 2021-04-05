#pragma once

#include <Windows.h>

class VirtualProtectGuard final
{
public:
	explicit VirtualProtectGuard(LPVOID address, SIZE_T size, DWORD protection);

	~VirtualProtectGuard();

	// Disable: copyable, cloneable, movable:
	VirtualProtectGuard(const VirtualProtectGuard&) = delete;
	VirtualProtectGuard& operator=(const VirtualProtectGuard&) = delete;
	VirtualProtectGuard(const VirtualProtectGuard&&) = delete;
	VirtualProtectGuard& operator=(const VirtualProtectGuard&&) = delete;

private:
	void restoreMemoryProtection();

	LPVOID m_address;
	SIZE_T m_size;
	DWORD m_oldProtection;
};

