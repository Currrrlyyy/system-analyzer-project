#include "stdafx.h"
#include "logger.h"
#include "utils.h"

std::optional<std::string> CLogger::s_LogFileName = std::nullopt;
std::mutex CLogger::s_Mutex;

void CLogger::Init(const std::string& csLogFileName)
{
    s_LogFileName.emplace(csLogFileName);
}

CLogger::CLogger()
{
    AddTimestamp();
    AddAccountName();
}

CLogger::~CLogger()
{
    if (!s_LogFileName)
    {
        return;
    }
    std::lock_guard<std::mutex> lg(s_Mutex);
    std::ofstream logFile(*s_LogFileName, std::ios_base::app);
    logFile << m_Buffer.str() << std::endl;
}

CLogger& CLogger::AsLValue()
{
    return *this;
}

void CLogger::AddTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time);
    if (!tm)
    {
        m_Buffer << "[ ] ";
    }
    else
    {
        m_Buffer << "[" << std::put_time(tm, "%Y-%m-%d %X") << "] ";
    }
}

void CLogger::AddAccountName()
{
    std::optional<std::string> accountName = utils::GetAccountName();
    if (!accountName)
    {
        m_Buffer << "[ ] ";
    }
    else
    {
        m_Buffer << "[" << *accountName << "] ";
    }
}
