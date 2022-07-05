#include "stdafx.h"
#include "logger.h"
#include "utils.h"

std::optional<std::string> CLogger::s_LogFileName = std::nullopt;
std::mutex CLogger::s_Mutex;

// Create log file in directory with given name
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

// Write system time in log file
void CLogger::AddTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    const std::tm* tm = std::localtime(&time);

    // If couldn't get system time
    if (!tm)
    {
        m_Buffer << "[] ";
    }
    // Otherwise
    else
    {
        m_Buffer << "[" << std::put_time(tm, "%Y-%m-%d %X") << "] ";
    }
}

// Write user name in log file
void CLogger::AddAccountName()
{
    m_Buffer << "[" << utils::GetAccountName().value_or("") << "] ";
}
