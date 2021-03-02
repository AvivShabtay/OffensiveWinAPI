#include "MemoryInformation.h"

MemoryInformation::MemoryInformation(DWORD processPid)
	: m_processPid(processPid)
{
}

MemoryInformationIterator MemoryInformation::begin() const
{
	return MemoryInformationIterator(this->m_processPid);
}

EndMemoryInformationIterator MemoryInformation::end() const
{
	return EndMemoryInformationIterator{};
}
