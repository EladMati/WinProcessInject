#include "Manager.h"
#include <iostream>

Manager::Manager(const std::wstring& dllPath, const std::wstring& logPath)
    : m_notepadInject(std::make_unique<NotepadInject>(dllPath, logPath))
    ,  m_calcKill(std::make_unique<CalcKill>())
{}

Manager::~Manager() 
{
    stop();

    if (m_notepadThread.joinable()) 
    {
        m_notepadThread.join();
    }
    if (m_calcThread.joinable()) 
    {
        m_calcThread.join();
    }
}

void Manager::run() 
{

    m_notepadThread = std::thread([this]() {
        try 
        {
            m_notepadInject->injectAll();
        } 
        catch (const std::exception& ex) 
        {
            std::cerr << "Exception in NotepadInject thread: " << ex.what() << std::endl;
        }
    });

    m_calcThread = std::thread([this]() {
        try 
        {
            m_calcKill->killAll();
        } 
        catch (const std::exception& ex) 
        {
            std::cerr << "Exception in CalcKill thread: " << ex.what() << std::endl;
        }
    });
    
    if (m_notepadThread.joinable()) 
    {
        m_notepadThread.join();
    }
    if (m_calcThread.joinable()) 
    {
        m_calcThread.join();
    }
}

void Manager::stop() 
{
    if (m_notepadInject)
    {
        m_notepadInject->stop();
    }

    if (m_calcKill)
    {
        m_calcKill->stop();
    }
}
