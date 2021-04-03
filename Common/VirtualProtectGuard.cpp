#include "VirtualProtectGuard.h"
#include "Win32ErrorCodeException.h"

#include <stdexcept>
#include <iostream>

VirtualProtectGuard::VirtualProtectGuard(LPVOID address, SIZE_T size, DWORD protection)
	: m_address(address), m_size(size), m_oldProtection(0)
{
	if (nullptr == this->m_address)
	{
		throw std::runtime_error("Invalid address");
	}

	if (0 >= this->m_size)
	{
		throw std::runtime_error("Invalid size");
	}

	if (!VirtualProtect(this->m_address, this->m_size, protection, &this->m_oldProtection))
	{
		throw Win32ErrorCodeException("Could not change memory protection");
	}
}

VirtualProtectGuard::~VirtualProtectGuard()
{
	try
	{
		this->restoreMemoryProtection();
	}
	catch (const Win32ErrorCodeException& exception)
	{
		std::cout << exception.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception thrown in destructor of: VirtualProtectGuard" << std::endl;
	}
}

void VirtualProtectGuard::restoreMemoryProtection()
{
	if (!VirtualProtect(this->m_address, this->m_size, this->m_oldProtection, &this->m_oldProtection))
	{
		throw Win32ErrorCodeException("Could not change memory protection");
	}
}
