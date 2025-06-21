#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <system_error>
#include <list>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include "Logger.h"
#include "NotepadInject.h"
#include "CalcKill.h"
#include "Manager.h"
#include <thread>

int main()
{
    const std::wstring dllPath = L"C:\\Temp\\TotallyNotAVirus.dll";
    const std::wstring logPath = L"C:\\Temp\\just_another_innocent_file.txt";

    Manager manager(dllPath, logPath);
    manager.run();
    
    return 0;
}

