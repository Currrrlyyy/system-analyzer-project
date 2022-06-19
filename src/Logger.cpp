#include <Logger.h>

#include <fstream>
#include <iomanip>

std::string CLogger::s_sLogFileName = "";
std::mutex CLogger::s_Mutex;

void CLogger::Init(const std::string& csLogFileName)
{
    s_sLogFileName = csLogFileName;
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
    if (s_sLogFileName == "")
    {
        return;
    }
    std::lock_guard<std::mutex> lg(s_Mutex);
    std::ofstream logFile(s_sLogFileName, std::ios_base::app);
    logFile << m_oss.str() << std::endl;
}

void CLogger::AddTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t tmp = std::chrono::system_clock::to_time_t(now);
    std::tm time = { 0 };
    if (localtime_s(&time, &tmp) != 0)
    {
        return;
    }
    m_oss << "[" << std::put_time(&time, "%Y-%m-%d %X") << "] ";
}
