#include "AutoVirtualAlloc.h"
#include "Win32ErrorCodeException.h"

#include <stdexcept>

AutoVirtualAlloc::AutoVirtualAlloc(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address)
	: m_allocationAddress(nullptr), m_allocationSize(size)
{
	this->allocate(size, allocationType, protection, address);
}

AutoVirtualAlloc::~AutoVirtualAlloc()
{
	try
	{
		this->free();
	}
	catch (const Win32ErrorCodeException& exception)
	{
		std::cout << exception.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception thrown in destructor of: AutoVirtualAlloc" << std::endl;
	}
}

void AutoVirtualAlloc::reset(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address)
{
	if (nullptr != this->m_allocationAddress)
	{
		this->free();
	}

	this->allocate(size, allocationType, protection, address);
}

void* AutoVirtualAlloc::get() const
{
	return this->m_allocationAddress;
}

void AutoVirtualAlloc::allocate(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address)
{
	if (0 >= size)
	{
		throw std::runtime_error("Invalid size");
	}

	this->m_allocationAddress = VirtualAlloc(address, size, allocationType, protection);
	if (nullptr == this->m_allocationAddress)
	{
		throw Win32ErrorCodeException("Could not allocate memory");
	}
}

void AutoVirtualAlloc::free() const
{
	if (nullptr != this->m_allocationAddress)
	{
		if (!VirtualFree(this->m_allocationAddress, this->m_allocationSize, MEM_RELEASE))
		{
			throw Win32ErrorCodeException("Could not free allocated memory");
		}
	}
}
