#include "ComInitializationGuard.h"
#include "Win32ErrorCodeException.h"
#include "Exception.h"

#include <comdef.h>

ComInitializationGuard::ComInitializationGuard(COINIT coInit)
{
	const HRESULT result = CoInitializeEx(nullptr, coInit);
	if (FAILED(result))
	{
		const _com_error error(result);
		THROW_WSTRING_RUNTIME_ERROR(std::wstring(error.ErrorMessage()));
	}
}

ComInitializationGuard::~ComInitializationGuard()
{
	try
	{
		if (S_OK == this->m_result)
		{
			CoUninitialize();
		}
	}
	catch (...)
	{
		std::cout << "Exception was thrown at ComInitializeGuard destructor" << std::endl;
	}
}
