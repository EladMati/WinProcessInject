#pragma once
#include <memory>
#include "NotepadInject.h"
#include "CalcKill.h"
#include <thread>

class Manager {
public:
    explicit Manager(const std::wstring& dllPath, const std::wstring& logPath);
    ~Manager();
    void run();
    void stop();

private:
    std::unique_ptr<NotepadInject> m_notepadInject;
    std::unique_ptr<CalcKill> m_calcKill;
    std::thread m_notepadThread;
    std::thread m_calcThread;
};
