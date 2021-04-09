#include "WmiProcessIterator.h"

#include <stdexcept>
#include <comutil.h>

WmiProcessIterator::WmiProcessIterator(IEnumWbemClassObject* classObjectEnumerator)
	: m_classObjectEnumerator(classObjectEnumerator), m_hasMoreData(true)
{
	if (nullptr == this->m_classObjectEnumerator)
	{
		throw std::runtime_error("Invalid class object enumerator");
	}

	// Initiate the first process details:
	this->getNext();
}

WmiProcessIterator& WmiProcessIterator::begin()
{
	return *this;
}

EndWmiProcessIterator WmiProcessIterator::end()
{
	return EndWmiProcessIterator{};
}

const WmiProcessDetails& WmiProcessIterator::operator*() const
{
	return this->m_currentProcessData;
}

WmiProcessIterator& WmiProcessIterator::operator++()
{
	this->getNext();
	return *this;
}

bool WmiProcessIterator::operator!=(EndWmiProcessIterator) const
{
	return this->m_hasMoreData;
}

void WmiProcessIterator::getNext()
{
	if (nullptr == this->m_classObjectEnumerator)
	{
		this->m_hasMoreData = false;
		return;
	}

	CComPtr<IWbemClassObject> classObject = nullptr;

	ULONG returnStatus = 0;
	HRESULT result = this->m_classObjectEnumerator->Next(
		WBEM_INFINITE,
		1,
		&classObject,
		&returnStatus
	);

	if (0 == returnStatus)
	{
		this->m_hasMoreData = false;
	}
	else
	{
		_variant_t processName;
		result = classObject->Get(L"Name", 0, &processName, nullptr, nullptr);
		m_currentProcessData.processName = processName.bstrVal;

		_variant_t processId;
		result = classObject->Get(L"ProcessId", 0, &processId, nullptr, nullptr);
		m_currentProcessData.processId = processId.uintVal;
	}
}
