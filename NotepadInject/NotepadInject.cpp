#include "NotepadInject.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <unordered_set>

namespace {
    constexpr const char* NOTEPAD_EXE = "notepad.exe";
}

NotepadInject::NotepadInject(const std::wstring& dllPath, const std::wstring& logPath)
    : m_dllPath(dllPath)
    , m_run(true)
{
    try 
    {
        m_logger = std::make_unique<Logger>(logPath);
    } 
    catch (const std::exception& ex) 
    {
        m_logger = nullptr;
        throw std::runtime_error("Logger initialization failed");
    }
}

bool NotepadInject::injectDLL(DWORD processId, const std::wstring& dllPath)
{
    if (!std::filesystem::exists(dllPath)) 
    {
        throw std::runtime_error("DLL path does not exist: " + std::string(dllPath.begin(), dllPath.end()));
    }
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) 
    {
        throw std::runtime_error("Failed to open process: " + std::to_string(processId) + " Error: " + std::to_string(GetLastError()));
    }

    // Allocate memory in the target process for the DLL path
    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remoteString = VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteString) 
    {
        CloseHandle(hProcess);
        return false;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, remoteString, dllPath.c_str(), pathSize, nullptr)) 
    {
        VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32)
    {
        VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    // Create a remote thread in the target process to load the DLL
    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibraryAddr, remoteString, 0, nullptr);
    if (hThread) 
    {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return hThread != nullptr;
}

void NotepadInject::stop() 
{
    m_run = false;
}

void NotepadInject::injectAll() 
{
    std::unordered_set<DWORD> injectedPIDs;
    while (m_run) 
    {
        DWORD processes[1024];
        DWORD needed;
        DWORD count;
        if (!EnumProcesses(processes, sizeof(processes), &needed)) 
        {
            std::cerr << "Failed to enumerate processes." << std::endl;
            return;
        }
        count = needed / sizeof(DWORD);
        for (unsigned int i = 0; i < count; ++i) 
        {
            if (processes[i] == 0) continue;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess) 
            {
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) 
                {
                    char szProcessName[MAX_PATH] = "";
                    GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(char));
                    //find all processes with notepad.exe name
                    if (_stricmp(szProcessName, NOTEPAD_EXE) == 0) 
                    {
                        //if a process is already injected, skip it
                        if (injectedPIDs.count(processes[i])) 
                        {
                            CloseHandle(hProcess);
                            continue;
                        }
                        injectedPIDs.insert(processes[i]);
                        if (std::filesystem::exists(m_dllPath)) 
                        {
                            if (injectDLL(processes[i], m_dllPath)) 
                            {
                                std::wcout << L"Successfully injected into PID: " << processes[i] << std::endl;
                                if (m_logger) m_logger->write(processes[i]);
                            } 
                            else 
                            {
                                std::wcout << L"Failed to inject into PID: " << processes[i] << std::endl;
                            }
                        } 
                        else 
                        {
                            throw std::runtime_error("DLL path does not exist: " + std::string(m_dllPath.begin(), m_dllPath.end()));
                        }
                    }
                }

                CloseHandle(hProcess);
            }
        }

        //Long time of 400 milliseconds since related tu user interaction
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
