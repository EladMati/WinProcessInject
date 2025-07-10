#pragma once
#include <atomic>
#include <chrono>
#include <string>
#include "ProcessTracker.h"

class CalcKill : public ProcessTracker {
public:
    explicit CalcKill(std::chrono::milliseconds sleepMs = std::chrono::milliseconds(300));

    void killAll();
    void stop();

protected:
    void OnProcessFound(DWORD processId, HANDLE hProcess, const std::string& processName) override;

private:
    std::atomic_bool m_run;
    std::chrono::milliseconds m_sleepMs;
};
