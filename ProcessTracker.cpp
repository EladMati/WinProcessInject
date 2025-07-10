#include "ProcessTracker.h"

ProcessTracker::ProcessTracker()
    : m_hProcess(nullptr)
{
}

ProcessTracker::~ProcessTracker()
{
    if (m_hProcess)
    {
        CloseHandle(m_hProcess);
        m_hProcess = nullptr;
    }
}

void ProcessTracker::SearchInAllProcesses(const std::string& processName)
{
    DWORD processes[1024], needed, count;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        std::cerr << "Failed to enumerate processes." << std::endl;
        return;
    }

    HMODULE hMod;
    DWORD cbNeeded;
    char szProcessName[MAX_PATH] = "";

    count = needed / sizeof(DWORD);
    for (unsigned int i = 0; i < count; ++i)
    {
        if (processes[i] == 0)
        {
            continue;
        }
        // Close previous handle if open
        if (m_hProcess)
        {
            CloseHandle(m_hProcess);
            m_hProcess = nullptr;
        }

        m_hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
        if (m_hProcess)
        {
            if (EnumProcessModules(m_hProcess, &hMod, sizeof(hMod), &cbNeeded))
            {
                GetModuleBaseNameA(m_hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(char));
                if (_stricmp(szProcessName, processName.c_str()) == 0)
                {
                    try
                    {
                        OnProcessFound(processes[i], m_hProcess, processName);
                    }
                    catch (const std::exception& ex)
                    {
                        std::cerr << "Exception in OnProcessFound: " << ex.what() << std::endl;
                    }
                }
            }
            // Don't close here; will be closed in destructor or before next open
        }
    }
}
