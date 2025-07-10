#include "CalcKill.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <thread>

CalcKill::CalcKill(std::chrono::milliseconds sleepMs)
    : m_run(true), m_sleepMs(sleepMs)
{
}

void CalcKill::killAll()
{
    while (m_run)
    {
        try
        {
            SearchInAllProcesses("CalculatorApp.exe");
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(m_sleepMs);
    }
}

void CalcKill::OnProcessFound(DWORD processId, HANDLE hProcess, const std::string& processName)
{
    std::cout << "Killing " << processName << " PID: " << processId << std::endl;
    TerminateProcess(hProcess, 1);
}

void CalcKill::stop()
{
    m_run = false;
}
