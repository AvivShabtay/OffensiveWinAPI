#include "AutoShutdownFile.h"

#include <sstream>

#include "../Common/AutoHandle.h"
#include "../Common/Win32ErrorCodeException.h"

AutoShutdownFile::AutoShutdownFile(std::wstring filename, std::wstring filepath)
	: m_filename(filename), m_filepath(filepath)
{
	this->createShutdownFile();
}

void AutoShutdownFile::createShutdownFile() const
{
	std::wstringstream stream;
	stream << this->m_filepath;
	stream << "\\";
	stream << this->m_filename;
	stream << SHUTDOWN_FILE_EXTENSION;

	AutoHandle file(CreateFile(stream.str().c_str(), GENERIC_READ | GENERIC_WRITE, 0,
		nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
	if (INVALID_HANDLE_VALUE == file.get())
	{
		throw Win32ErrorCodeException("Could not create shutdown file");
	}

	DWORD bytesWrite = 0;
	if (!WriteFile(file.get(), SHUTDOWN_COMMAND.data(), SHUTDOWN_COMMAND.size(), &bytesWrite, nullptr))
	{
		throw Win32ErrorCodeException("Could not write command to shutdown file");
	}
}
