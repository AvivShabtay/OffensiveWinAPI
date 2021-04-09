#include "WmiQuery.h"
#include "Exception.h"

#include <comdef.h>

WmiQuery::WmiQuery(const std::wstring& wmiNamespace, const std::wstring& wmiQueryLanguage)
	: m_wmiNamespace(wmiNamespace.c_str()), m_wmiQueryLanguage(wmiQueryLanguage.c_str()), m_classObjectEnumerator(nullptr)
{
	HRESULT result = CoCreateInstance(CLSID_WbemLocator, nullptr,
		CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&this->m_locator));

	if (FAILED(result))
	{
		const _com_error error(result);
		THROW_WSTRING_RUNTIME_ERROR(std::wstring(error.ErrorMessage()));
	}

	result = this->m_locator->ConnectServer(
		this->m_wmiNamespace,	// Object path of WMI namespace
		nullptr,                    // User name. NULL = current user
		nullptr,                    // User password. NULL = current
		nullptr,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		nullptr,                       // Authority (for example, Kerberos)
		nullptr,                       // Context object
		&this->m_service	// pointer to IWbemServices proxy
	);

	if (FAILED(result))
	{
		const _com_error error(result);
		THROW_WSTRING_RUNTIME_ERROR(std::wstring(error.ErrorMessage()));
	}

	// Set security levels on the proxy:
	result = CoSetProxyBlanket(
		this->m_service,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                        // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                        // client identity
		EOAC_NONE                    // proxy capabilities
	);

	if (FAILED(result))
	{
		const _com_error error(result);
		THROW_WSTRING_RUNTIME_ERROR(std::wstring(error.ErrorMessage()));
	}
}

IEnumWbemClassObject* WmiQuery::executeQuery(const std::wstring& query)
{
	const HRESULT result = this->m_service->ExecQuery(
		this->m_wmiQueryLanguage,
		bstr_t(query.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&this->m_classObjectEnumerator);

	if (FAILED(result))
	{
		const _com_error error(result);
		THROW_WSTRING_RUNTIME_ERROR(std::wstring(error.ErrorMessage()));
	}

	return this->m_classObjectEnumerator;
}
