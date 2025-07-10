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

NotepadInject::NotepadInject(const std::wstring& dllPath,
    const std::wstring& logPath,
    std::chrono::milliseconds sleepMs)
    : m_dllPath(dllPath)
    , m_logger(nullptr)
    , m_run(true)
    , m_sleepMs(sleepMs)
    , m_hInjectProcess(nullptr)
    , m_hInjectThread(nullptr)
    , m_hKernel32(nullptr)
    , m_remoteString(nullptr)
{
    try
    {
        m_logger = std::make_unique<Logger>(logPath);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Logger initialization failed");
    }
}

NotepadInject::~NotepadInject()
{
    if (m_hInjectThread)
    {
        CloseHandle(m_hInjectThread);
        m_hInjectThread = nullptr;
    }

    if (m_hInjectProcess)
    {
        CloseHandle(m_hInjectProcess);
        m_hInjectProcess = nullptr;
    }

    if (m_remoteString && m_hInjectProcess)
    {
        VirtualFreeEx(m_hInjectProcess, m_remoteString, 0, MEM_RELEASE);
        m_remoteString = nullptr;
    }
}

void NotepadInject::injectAll()
{
    std::unordered_set<DWORD> injectedPIDs;

    while (m_run)
    {
        try
        {
            SearchInAllProcesses(NOTEPAD_EXE);
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }

        std::this_thread::sleep_for(m_sleepMs);
    }
}

void NotepadInject::OnProcessFound(DWORD processId, HANDLE hProcess, const std::string& processName)
{
    if (m_logger)
    {
        m_logger->write(processId);
    }

    if (!injectDLL(processId, m_dllPath))
    {
        throw std::runtime_error("DLL injection failed for process: " + std::to_string(processId));
    }
}

void NotepadInject::stop()
{
    m_run = false;
}

bool NotepadInject::injectDLL(DWORD processId, const std::wstring& dllPath)
{
    if (!std::filesystem::exists(dllPath))
    {
        throw std::runtime_error("DLL path does not exist: " + std::string(dllPath.begin(), dllPath.end()));
    }

    if (m_hInjectThread)
    {
        CloseHandle(m_hInjectThread);
        m_hInjectThread = nullptr;
    }

    if (m_hInjectProcess)
    {
        CloseHandle(m_hInjectProcess);
        m_hInjectProcess = nullptr;
    }

    if (m_remoteString && m_hInjectProcess)
    {
        VirtualFreeEx(m_hInjectProcess, m_remoteString, 0, MEM_RELEASE);
        m_remoteString = nullptr;
    }

    m_hInjectProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);

    if (!m_hInjectProcess)
    {
        throw std::runtime_error("Failed to open process: " + std::to_string(processId) +
            " Error: " + std::to_string(GetLastError()));
    }

    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    m_remoteString = VirtualAllocEx(m_hInjectProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!m_remoteString)
    {
        CloseHandle(m_hInjectProcess);
        m_hInjectProcess = nullptr;
        return false;
    }

    if (!WriteProcessMemory(m_hInjectProcess, m_remoteString, dllPath.c_str(), pathSize, nullptr))
    {
        VirtualFreeEx(m_hInjectProcess, m_remoteString, 0, MEM_RELEASE);
        m_remoteString = nullptr;
        CloseHandle(m_hInjectProcess);
        m_hInjectProcess = nullptr;
        return false;
    }

    m_hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!m_hKernel32)
    {
        VirtualFreeEx(m_hInjectProcess, m_remoteString, 0, MEM_RELEASE);
        m_remoteString = nullptr;
        CloseHandle(m_hInjectProcess);
        m_hInjectProcess = nullptr;
        return false;
    }

    auto loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(m_hKernel32, "LoadLibraryW");
    m_hInjectThread = CreateRemoteThread(m_hInjectProcess, nullptr, 0, loadLibraryAddr, m_remoteString, 0, nullptr);

    if (m_hInjectThread)
    {
        WaitForSingleObject(m_hInjectThread, INFINITE);
        CloseHandle(m_hInjectThread);
        m_hInjectThread = nullptr;
    }

    VirtualFreeEx(m_hInjectProcess, m_remoteString, 0, MEM_RELEASE);
    m_remoteString = nullptr;
    CloseHandle(m_hInjectProcess);
    m_hInjectProcess = nullptr;

    return m_hInjectThread != nullptr;
}
