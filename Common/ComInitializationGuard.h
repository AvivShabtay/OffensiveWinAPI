#pragma once

#include <atlalloc.h>

/*
 * @note If you are using Smart pointer such as: CComPtr<>, their lifetime must end BEFORE
 * CoUnitialize (the destructor of this guard) is called, so use { } as scope, see @usage.
 * @usage:
		ComInitializationGuard comInitializationGuard;
		{
			// ... do something using COM Smart pointers
		}
 */
class ComInitializationGuard
{
public:
	explicit ComInitializationGuard(COINIT coInit = COINIT_MULTITHREADED);

	virtual ~ComInitializationGuard();

	// Disable: copyable, assignment, movable:
	ComInitializationGuard(const ComInitializationGuard&) = delete;
	ComInitializationGuard& operator=(const ComInitializationGuard&) = delete;
	ComInitializationGuard(ComInitializationGuard&&) = delete;
	ComInitializationGuard& operator=(ComInitializationGuard&&) = delete;

private:
	HRESULT m_result;
};

