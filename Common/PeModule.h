#pragma once

#include <string>
#include <vector>
#include <map>

#include <Windows.h>

// This macro will help with calculating addresses
#define PtrFromRva(base, rva) (((PBYTE)base) + rva)

class PeModule
{
public:
	explicit PeModule(const HMODULE module);

	virtual ~PeModule() = default;

	/*
	 * Return pointer to the PE DOS header.
	 */
	PIMAGE_DOS_HEADER getDosHeader() const;

	/*
	 * Return pointer to the PE NT headers.
	 */
	PIMAGE_NT_HEADERS getNtHeaders() const;

	/*
	 * Return pointer to the PE Optional header.
	 */
	PIMAGE_OPTIONAL_HEADER getOptionalHeader() const;

	/*
	 * Return all the import descriptor of the PE module.
	 */
	std::vector<PIMAGE_IMPORT_DESCRIPTOR> getImportDescriptors() const;

	/*
	 * Return pair of: module name and pointer to the module descriptor, of all the modules that
	 * PE module import from them.
	 */
	std::map<std::string, PIMAGE_IMPORT_DESCRIPTOR> getModules() const;

	/*
	 * Search and return the IMAGE_THUNK_DATA first thunk of import function name from given module descriptor.
	 */
	PIMAGE_THUNK_DATA getImportImageThunkData(PIMAGE_IMPORT_DESCRIPTOR moduleDescriptor,
		const std::string& importFunctionName) const;

	/*
	 * Search and return the import function address from given module descriptor.
	 */
	void* getImportFunctionAddress(PIMAGE_IMPORT_DESCRIPTOR moduleDescriptor,
		const std::string& importFunctionName) const;

	/*
	 * Search for import function name from given module name and return it's address.
	 */
	void* getImportFunctionAddress(const std::string& moduleName, const std::string& importName) const;

private:
	/*
	 * Validates the DOS header and set the DOS header member.
	 */
	void setDosHeader(DWORD_PTR moduleBaseAddress);

	/*
	 * Validates the NT headers and set the NT headers member.
	 */
	void setNtHeaders(DWORD_PTR moduleBaseAddress, PIMAGE_DOS_HEADER dosHeader);

	/*
	 * Validates the Optional header and set the Optional header member.
	 */
	void setOptionalHeader(PIMAGE_NT_HEADERS ntHeaders);

	DWORD_PTR m_moduleBaseAddress;
	PIMAGE_DOS_HEADER m_dosHeader;
	PIMAGE_NT_HEADERS m_ntHeaders;
	PIMAGE_OPTIONAL_HEADER m_optionalHeader;
};

