#pragma once

#include "Wmi.h"

#include <string>

#include <wbemcli.h>
#include <atlbase.h>
#include <comutil.h>

class WmiQuery
{
public:
	explicit WmiQuery(const std::wstring& wmiNamespace = WMI_ROOT_CIMV2_NAMESPACE, const std::wstring& wmiQueryLanguage = WMI_WQL_QUERY_LANGUAGE);

	virtual ~WmiQuery() = default;

	IEnumWbemClassObject* executeQuery(const std::wstring& query);

private:
	_bstr_t m_wmiNamespace;
	_bstr_t m_wmiQueryLanguage;
	CComPtr<IWbemLocator> m_locator;
	CComPtr<IWbemServices> m_service;
	CComPtr<IEnumWbemClassObject> m_classObjectEnumerator;
};
