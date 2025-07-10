#include "Logger.h"
#include <iostream>
#include <minwindef.h>
#include <stdexcept>

Logger::Logger(const std::wstring& filePath) 
{
    std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
    m_pidFile.open(filePath, std::ios::app);
    if (!m_pidFile) 
    {
        throw std::runtime_error("Failed to open file for writing: " + std::string(filePath.begin(), filePath.end()));
    }
}

Logger::~Logger() 
{
    if (m_pidFile) 
    {
        m_pidFile.close();
    }
}

void Logger::write(DWORD pid) 
{
    if (m_pidFile) 
    {
        m_pidFile << pid << std::endl;
    }
}
