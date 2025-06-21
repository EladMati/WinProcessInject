#include "CalcKill.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <thread>

CalcKill::CalcKill() 
    : m_run(true) 
{}

void CalcKill::killAll() 
{
    while (m_run) 
    {
        DWORD processes[1024], needed, count;
        if (!EnumProcesses(processes, sizeof(processes), &needed)) 
        {
            std::cerr << "Failed to enumerate processes." << std::endl;
            return;
        }

        count = needed / sizeof(DWORD);
        for (unsigned int i = 0; i < count; ++i) 
        {
            if (processes[i] == 0) 
            {
                continue;
            }

            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess) 
            {
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) 
                {
                    char szProcessName[MAX_PATH] = "";
                    GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(char));
                    // Check if the process name is "calc.exe"
                    if (_stricmp(szProcessName, "calc.exe") == 0) 
                    {
                        std::cout << "Killing calc.exe PID: " << processes[i] << std::endl;
                        TerminateProcess(hProcess, 1);
                    }
                }

                CloseHandle(hProcess);
            }
        }

        // long time of 400 milliseconds since related tu user interaction
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void CalcKill::stop() 
{
    m_run = false;
}
