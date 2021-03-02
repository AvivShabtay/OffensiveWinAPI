#pragma once

#include "MemoryInformationIterator.h"

#include <Windows.h>

class MemoryInformation
{
public:
	MemoryInformation(DWORD processPid);

	virtual ~MemoryInformation() = default;

	// delete copy ctor, move ctor, assignment
	MemoryInformation(const MemoryInformation&) = delete;
	MemoryInformation& operator=(const MemoryInformation&) = delete;
	MemoryInformation(MemoryInformation&&) = delete;
	MemoryInformation& operator=(MemoryInformation&&) = delete;

	/*
	 * Start iterator instance.
	 * @return Iterator for virtual memory information.
	 */
	MemoryInformationIterator begin() const;

	/*
	 * Indicating the end of the iteration.
	 * @note Different types between begin() and end() available from C++17. https://stackoverflow.com/a/62716532
	 * @return Dummy class.
	 */
	EndMemoryInformationIterator end() const;

private:
	DWORD m_processPid;
};

