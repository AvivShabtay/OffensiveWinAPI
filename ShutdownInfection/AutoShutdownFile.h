#pragma once

#include <iostream>

const std::string SHUTDOWN_COMMAND("shutdown /s /f /t 0");
const std::wstring SHUTDOWN_FILE_EXTENSION(L".bat");

class AutoShutdownFile
{
public:
	AutoShutdownFile(std::wstring filename, std::wstring filepath);

private:
	void createShutdownFile() const;

	std::wstring m_filename;
	std::wstring m_filepath;
};

