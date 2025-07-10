#pragma once
#include <windows.h> 
#include <fstream>
#include <filesystem>
#include <string>

class Logger {
public:
    Logger(const std::wstring& filePath);
    ~Logger();
    void write(DWORD pid);
private:
    std::wofstream m_pidFile;
};
