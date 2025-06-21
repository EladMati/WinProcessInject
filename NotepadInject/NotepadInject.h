#pragma once
#include <memory>
#include <string>
#include <atomic>
#include "Logger.h"

class NotepadInject
{
public:
    NotepadInject(const std::wstring& dllPath, const std::wstring& logPath);
    void injectAll();
    void stop();
    
private:
    void findNotepadProcessesAndInject();
    bool injectDLL(DWORD processId, const std::wstring& dllPath);
    std::wstring m_dllPath;
    std::unique_ptr<Logger> m_logger;
    std::atomic_bool m_run;
};

