#pragma once

#include "ProcessTracker.h"
#include "Logger.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <windows.h>

class NotepadInject : public ProcessTracker
{
public:
    explicit NotepadInject(const std::wstring& dllPath,
        const std::wstring& logPath,
        std::chrono::milliseconds sleepMs = std::chrono::milliseconds(300));
    ~NotepadInject();

    void injectAll();
    void stop();

protected:
    void OnProcessFound(DWORD processId, HANDLE hProcess, const std::string& processName) override;

private:
    bool injectDLL(DWORD processId, const std::wstring& dllPath);

    std::wstring m_dllPath;
    std::unique_ptr<Logger> m_logger;
    std::atomic_bool m_run;
    std::chrono::milliseconds m_sleepMs;

    // RAII handles for DLL injection
    HANDLE m_hInjectProcess;
    HANDLE m_hInjectThread;
    HMODULE m_hKernel32;
    LPVOID m_remoteString;
};
