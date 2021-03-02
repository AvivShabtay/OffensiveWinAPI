#pragma once

#include <stdexcept>
#include <iostream>

#include <Windows.h>

class Win32ErrorCodeException : public std::runtime_error
{
public:
	explicit Win32ErrorCodeException(const std::string& errorMessage);

	virtual ~Win32ErrorCodeException() = default;

	const char* what() const override;

	// delete copy ctor, move ctor, assignment
	Win32ErrorCodeException(const Win32ErrorCodeException&) = delete;
	Win32ErrorCodeException& operator=(const Win32ErrorCodeException&) = delete;
	Win32ErrorCodeException(Win32ErrorCodeException&&) = delete;
	Win32ErrorCodeException& operator=(Win32ErrorCodeException&&) = delete;

	/* Get Windows last error code. */
	DWORD getErrorCode() const;

	/* Get Windows message corresponding to the last error that occurred. */
	std::string getWinErrorMessage() const;

protected:
	std::string getLastErrorMessage() const;

	DWORD m_errorCode;
	std::string m_winErrorMessage;
	std::string m_errorMessage;
};

