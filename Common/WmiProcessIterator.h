#pragma once

#include <string>
#include <Wbemidl.h>
#include <atlbase.h>

class EndWmiProcessIterator {};

struct WmiProcessDetails
{
	std::wstring processName;
	std::uint32_t processId;
};

class WmiProcessIterator
{
public:
	explicit WmiProcessIterator(IEnumWbemClassObject* classObjectEnumerator);

	virtual ~WmiProcessIterator() = default;

	WmiProcessIterator& begin();

	static EndWmiProcessIterator end();

	const WmiProcessDetails& operator*() const;

	WmiProcessIterator& operator++();

	bool operator!=(EndWmiProcessIterator) const;

private:
	void getNext();

	CComPtr<IEnumWbemClassObject> m_classObjectEnumerator;
	WmiProcessDetails m_currentProcessData;
	bool m_hasMoreData;
};

