#pragma once

#include <Windows.h>

class AutoVirtualAlloc
{
public:
	AutoVirtualAlloc() = default;

	explicit AutoVirtualAlloc(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address = nullptr);

	~AutoVirtualAlloc();

	// Disable: copyable, cloneable, movable:
	AutoVirtualAlloc(const AutoVirtualAlloc&) = delete;
	AutoVirtualAlloc& operator=(const AutoVirtualAlloc&) = delete;
	AutoVirtualAlloc(const AutoVirtualAlloc&&) = delete;
	AutoVirtualAlloc& operator=(const AutoVirtualAlloc&&) = delete;

	void reset(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address = nullptr);

	void* get() const;

private:
	void allocate(SIZE_T size, DWORD allocationType, DWORD protection, LPVOID address = nullptr);
	void free() const;

	void* m_allocationAddress;
	SIZE_T m_allocationSize;
};

