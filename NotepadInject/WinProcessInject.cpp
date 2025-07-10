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

#include <string>

constexpr const wchar_t* DEFAULT_DLL_PATH = L"C:\\Temp\\TotallyNotAVirus.dll";
constexpr const wchar_t* DEFAULT_LOG_PATH = L"C:\\Temp\\just_another_innocent_file.txt";

int wmain(int argc, wchar_t* argv[])
{
    std::wstring dllPath = (argc > 1) ? argv[1] : DEFAULT_DLL_PATH;
    std::wstring logPath = (argc > 2) ? argv[2] : DEFAULT_LOG_PATH;

    long ms = 0;
    if (argc > 3)
    {
        ms = std::wcstol(argv[3], nullptr, 10);
    }

    if (ms > 0)
    {
        Manager manager(dllPath, logPath, std::chrono::milliseconds(ms));
        manager.run();
    }
    else
    {
        // uses dafault sleep duration in Ctor
        Manager manager(dllPath, logPath);

        manager.run();
    }

    return 0;
}


