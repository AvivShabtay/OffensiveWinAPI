#pragma once

#include "../Common/AutoHandle.h"

#include <Windows.h>

/*
 * Used as an indication the iterator have no more iterations.
 */
class EndMemoryInformationIterator {};

static constexpr ULONGLONG MEMORY_BASIC_INFORMATION_SIZE = sizeof(MEMORY_BASIC_INFORMATION);

/*
 * Provides iterator for retrieving information about the virtual memory of specific process.
 */
class MemoryInformationIterator
{
public:
	MemoryInformationIterator(DWORD processPid);

	// delete copy ctor, move ctor, assignment
	MemoryInformationIterator(const MemoryInformationIterator&) = delete;
	MemoryInformationIterator& operator=(const MemoryInformationIterator&) = delete;
	MemoryInformationIterator(MemoryInformationIterator&&) = delete;
	MemoryInformationIterator& operator=(MemoryInformationIterator&&) = delete;

	virtual ~MemoryInformationIterator() = default;

	const MEMORY_BASIC_INFORMATION& operator*() const;

	MemoryInformationIterator& operator++();

	bool operator!=(EndMemoryInformationIterator) const;

private:
	/* Verify if there are more virtual memory to query, if not setting flag.*/
	void getNextMemoryBasicInformation();

	AutoHandle m_process;
	MEMORY_BASIC_INFORMATION m_memoryBasicInformation;
	PVOID m_memoryAddress;
	bool m_noMoreMemory;
};