#include "MemoryInformationIterator.h"
#include "../Common/Win32ErrorCodeException.h"

MemoryInformationIterator::MemoryInformationIterator(DWORD processPid)
	: m_memoryBasicInformation{ 0 }, m_memoryAddress(nullptr), m_noMoreMemory(false)
{
	this->m_process.reset(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processPid));
	if (INVALID_HANDLE_VALUE == this->m_process.get())
	{
		throw Win32ErrorCodeException("Could not open handle to process");
	}

	if (0 == VirtualQueryEx(this->m_process.get(), nullptr, &this->m_memoryBasicInformation, MEMORY_BASIC_INFORMATION_SIZE))
	{
		throw Win32ErrorCodeException("Could read process virtual memory");
	}

	this->m_memoryAddress = this->m_memoryBasicInformation.AllocationBase;
}

MemoryInformationIterator& MemoryInformationIterator::operator++()
{
	this->getNextMemoryBasicInformation();
	return *this;
}

bool MemoryInformationIterator::operator!=(EndMemoryInformationIterator) const
{
	return !this->m_noMoreMemory;
}

const MEMORY_BASIC_INFORMATION& MemoryInformationIterator::operator*() const
{
	return this->m_memoryBasicInformation;
}

void MemoryInformationIterator::getNextMemoryBasicInformation()
{
	// Get memory info about the current block of memory
	if (MEMORY_BASIC_INFORMATION_SIZE !=
		VirtualQueryEx(this->m_process.get(), this->m_memoryAddress, &this->m_memoryBasicInformation, MEMORY_BASIC_INFORMATION_SIZE))
	{
		this->m_noMoreMemory = true;
	}

	// Set the address of the next virtual memory to be queried:
	this->m_memoryAddress = static_cast<PVOID>(static_cast<PBYTE>(this->m_memoryAddress) + this->m_memoryBasicInformation.RegionSize);
}
