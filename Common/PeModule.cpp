#include "PeModule.h"

#include <stdexcept>

PeModule::PeModule(const HMODULE module)
	: m_moduleBaseAddress(0), m_dosHeader(nullptr), m_ntHeaders(nullptr), m_optionalHeader(nullptr)
{
	if (nullptr == module)
	{
		throw std::runtime_error("Invalid module handle");
	}

	this->m_moduleBaseAddress = reinterpret_cast<DWORD_PTR>(module);
	this->setDosHeader(this->m_moduleBaseAddress);
	this->setNtHeaders(this->m_moduleBaseAddress, this->m_dosHeader);
	this->setOptionalHeader(this->m_ntHeaders);
}

PIMAGE_DOS_HEADER PeModule::getDosHeader() const
{
	return this->m_dosHeader;
}

PIMAGE_NT_HEADERS PeModule::getNtHeaders() const
{
	return this->m_ntHeaders;
}

PIMAGE_OPTIONAL_HEADER PeModule::getOptionalHeader() const
{
	return this->m_optionalHeader;
}

std::vector<PIMAGE_IMPORT_DESCRIPTOR> PeModule::getImportDescriptors() const
{
	auto* const importDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
		PtrFromRva(
			this->m_moduleBaseAddress,
			this->m_ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
		));

	std::vector<PIMAGE_IMPORT_DESCRIPTOR> importDescriptors;

	// Characteristics = 0 represent null-import-descriptor:
	for (UINT uIndex = 0; importDescriptor[uIndex].Characteristics != 0; uIndex++)
	{
		importDescriptors.push_back(&importDescriptor[uIndex]);
	}

	return importDescriptors;
}

std::map<std::string, PIMAGE_IMPORT_DESCRIPTOR> PeModule::getModules() const
{
	std::vector<PIMAGE_IMPORT_DESCRIPTOR> importDescriptors = this->getImportDescriptors();

	std::map<std::string, PIMAGE_IMPORT_DESCRIPTOR> modules;

	for (const auto& importDescriptor : importDescriptors)
	{
		std::string moduleName(reinterpret_cast<const char*>(PtrFromRva(this->m_moduleBaseAddress, importDescriptor->Name)));
		modules.insert({ moduleName , importDescriptor });
	}

	return modules;
}

PIMAGE_THUNK_DATA PeModule::getImportImageThunkData(PIMAGE_IMPORT_DESCRIPTOR const moduleDescriptor,
	const std::string& importFunctionName) const
{
	auto* firstThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(PtrFromRva(this->m_moduleBaseAddress, moduleDescriptor->FirstThunk));
	auto* OriginalFirstThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(PtrFromRva(this->m_moduleBaseAddress, moduleDescriptor->OriginalFirstThunk));

	while (0 != OriginalFirstThunk->u1.Function)
	{
		// Search by name and not by ordinal:
		if (!(OriginalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
		{
			auto* importByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(PtrFromRva(this->m_moduleBaseAddress, OriginalFirstThunk->u1.AddressOfData));
			std::string functionName(static_cast<const char*>(importByName->Name));

			if (0 == importFunctionName.compare(functionName))
			{
				return firstThunk;
			}
		}

		OriginalFirstThunk++;
		firstThunk++;
	}

	throw std::runtime_error("Could not find the import function IMAGE_THUNK_DATA first thunk");
}

void* PeModule::getImportFunctionAddress(PIMAGE_IMPORT_DESCRIPTOR moduleDescriptor,
	const std::string& importFunctionName) const
{
	auto* const firstThunk = this->getImportImageThunkData(moduleDescriptor, importFunctionName);
	return &firstThunk->u1.Function;
}

void* PeModule::getImportFunctionAddress(const std::string& moduleName, const std::string& importName) const
{
	std::map<std::string, PIMAGE_IMPORT_DESCRIPTOR> modules = this->getModules();
	for (const auto& [module, moduleDescriptor] : modules)
	{
		if (0 == moduleName.compare(module))
		{
			return this->getImportFunctionAddress(moduleDescriptor, importName);
		}
	}

	throw std::runtime_error("Could not find the import function address in the IAT");
}

void PeModule::setDosHeader(DWORD_PTR moduleBaseAddress)
{
	this->m_dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddress);

	if (nullptr == this->m_dosHeader)
	{
		throw std::runtime_error("Invalid DOS header");
	}

	if (IMAGE_DOS_SIGNATURE != this->m_dosHeader->e_magic)
	{
		throw std::runtime_error("Invalid DOS header magic");
	}
}

void PeModule::setNtHeaders(DWORD_PTR moduleBaseAddress, PIMAGE_DOS_HEADER dosHeader)
{
	this->m_ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(this->m_moduleBaseAddress + this->m_dosHeader->e_lfanew);

	if (nullptr == this->m_ntHeaders)
	{
		throw std::runtime_error("Invalid NT headers");
	}

	if (IMAGE_NT_SIGNATURE != this->m_ntHeaders->Signature)
	{
		throw std::runtime_error("Invalid NT headers magic");
	}
}

void PeModule::setOptionalHeader(PIMAGE_NT_HEADERS ntHeaders)
{
	this->m_optionalHeader = &this->m_ntHeaders->OptionalHeader;

	if (nullptr == this->m_optionalHeader)
	{
		throw std::runtime_error("Invalid Optional header");
	}

	// TODO: Validates Optional header magic
}
