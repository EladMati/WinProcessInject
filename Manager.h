#pragma once

#include <memory>
#include <string>
#include <thread>
#include <chrono>

class NotepadInject;
class CalcKill;

class Manager {
public:
    explicit Manager(const std::wstring& dllPath, const std::wstring& logPath, std::chrono::milliseconds sleepMs = std::chrono::milliseconds(300));
    ~Manager();

    void run();
    void stop();

private:
    std::unique_ptr<NotepadInject> m_notepadInject;
    std::unique_ptr<CalcKill> m_calcKill;
    std::thread m_notepadThread;
    std::thread m_calcThread;
};
