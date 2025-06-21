#pragma once
#include <string>
#include <atomic>

class CalcKill {
public:
    CalcKill();
    void killAll();
    void stop();
private:
    std::atomic_bool m_run;
};
