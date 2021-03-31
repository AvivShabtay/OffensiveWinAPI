#include "VirtualAllocExGuard.h"
#include "../Common/Win32ErrorCodeException.h"

#include <stdexcept>

VirtualAllocExGuard::VirtualAllocExGuard(const HANDLE& process, SIZE_T allocationSize, DWORD protection,
	DWORD allocationType, LPVOID address)
	: m_process(process), m_address(nullptr), m_allocationSize(allocationSize)
{
	if (INVALID_HANDLE_VALUE == this->m_process)
	{
		throw std::runtime_error("Invalid process handle");
	}

	if (0 >= this->m_allocationSize)
	{
		throw std::runtime_error("Invalid allocation size");
	}

	this->m_address = VirtualAllocEx(this->m_process, address, allocationSize, allocationType, protection);
	if (nullptr == this->m_address)
	{
		throw Win32ErrorCodeException("Could not allocate memory in remote process");
	}
}

VirtualAllocExGuard::~VirtualAllocExGuard()
{
	try
	{
		this->free();
	}
	catch (Win32ErrorCodeException& exception)
	{
		std::cout << exception.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception occurred in destructor" << std::endl;
	}
}

LPVOID VirtualAllocExGuard::get() const
{
	return this->m_address;
}

void VirtualAllocExGuard::release()
{
	// Internationally prevent from free() to work:
	this->m_address = nullptr;
}

void VirtualAllocExGuard::free() const
{
	if (nullptr != this->m_address)
	{
		if (!VirtualFreeEx(this->m_process, this->m_address, this->m_allocationSize, MEM_DECOMMIT))
		{
			throw Win32ErrorCodeException("Could not free the memory");
		}
	}
}
