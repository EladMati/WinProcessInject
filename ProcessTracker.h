#pragma once
#include <windows.h>
#include <psapi.h>
#include <string>
#include <iostream>

class ProcessTracker {
public:
    ProcessTracker();
    virtual ~ProcessTracker();
    void SearchInAllProcesses(const std::string& processName);
protected:
    // Called when a process with the given name is found
    virtual void OnProcessFound(DWORD processId, HANDLE hProcess, const std::string& processName) = 0;
    HANDLE m_hProcess;
};
