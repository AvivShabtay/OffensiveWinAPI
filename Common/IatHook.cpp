#include "IatHook.h"
#include "DebugPrint.h"
#include "VirtualProtectGuard.h"
#include "PeModule.h"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <vector>

IatHook::IatHook(std::string moduleName, std::string functionName, void* hookingFunction)
	: m_moduleName(moduleName), m_functionName(functionName), m_hookingFunction(hookingFunction), m_originalImportAddressValue(0)
{
	const HMODULE currentProcessModule = GetModuleHandleA(nullptr);
	const PeModule pe(currentProcessModule);

	this->m_importFunctionAddress = pe.getImportFunctionAddress(this->m_moduleName, this->m_functionName);
	if (nullptr == this->m_importFunctionAddress)
	{
		throw std::runtime_error("Invalid import function address");
	}

	this->m_originalImportAddressValue = *static_cast<PULONGLONG>(this->m_importFunctionAddress);
}

IatHook::~IatHook()
{
	try
	{
		this->restoreOriginalFunction();
	}
	catch (const std::exception& exception)
	{
		DEBUG_PRINT(exception.what());
	}
	catch (...)
	{
		DEBUG_PRINT("Exception thrown in destructor of: IatHook");
	}
}

void IatHook::installHook() const
{
	VirtualProtectGuard virtualProtectGuard(this->m_importFunctionAddress, sizeof(void*), PAGE_EXECUTE_READWRITE);

	memcpy_s(this->m_importFunctionAddress, sizeof(DWORD), &this->m_hookingFunction, sizeof(DWORD));
}

void* IatHook::getOriginalAddress() const
{
	return reinterpret_cast<void*>(this->m_originalImportAddressValue);
}

void IatHook::restoreOriginalFunction() const
{
	VirtualProtectGuard virtualProtectGuard(this->m_importFunctionAddress, sizeof(void*), PAGE_EXECUTE_READWRITE);

	memcpy_s(this->m_importFunctionAddress, sizeof(void*), &this->m_originalImportAddressValue, sizeof(void*));
}