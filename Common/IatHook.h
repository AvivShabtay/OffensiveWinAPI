#pragma once

#include <string>
#include <Windows.h>

class IatHook
{
public:
	explicit IatHook(std::string moduleName, std::string functionName, void* hookingFunction);

	virtual ~IatHook();

	void installHook() const;

	void* getOriginalAddress() const;

private:
	void restoreOriginalFunction() const;

	std::string m_moduleName;
	std::string m_functionName;
	void* m_hookingFunction;
	void* m_importFunctionAddress;
	ULONGLONG m_originalImportAddressValue;
};

