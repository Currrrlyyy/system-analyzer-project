#include "stdafx.h"
#include "logger.h"

std::optional<std::string> CLogger::s_sLogFileName = std::nullopt;
std::mutex CLogger::s_Mutex;

void CLogger::Init(const std::string& csLogFileName)
{
    s_sLogFileName.emplace(csLogFileName);
}

CLogger::CLogger()
{
    AddTimestamp();
}

CLogger& CLogger::AsLValue()
{
    return *this;
}

CLogger::~CLogger()
{
    if (!s_sLogFileName)
    {
        return;
    }
    std::lock_guard<std::mutex> lg(s_Mutex);
    std::ofstream logFile(*s_sLogFileName, std::ios_base::app);
    logFile << m_oss.str() << std::endl;
}

void CLogger::AddTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time);
    if (tm == nullptr)
    {
        m_oss << "[can not get system time] ";
    }
    else
    {
        m_oss << "[" << std::put_time(tm, "%Y-%m-%d %X") << "] ";
    }
}
