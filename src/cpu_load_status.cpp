#include "stdafx.h"
#include "utils.h"
#include "logger.h"
#include "abstract_thread.h"
#include "cpu_load_status.h"



CCpuLoadStatus::CCpuLoadStatus(int iCriticalLoadValue, int iCpuLoadDelay):
    CBaseThread(iCpuLoadDelay),
    m_iCriticalLoadValue(iCriticalLoadValue)
{
}

void CCpuLoadStatus::Execute(std::future<void> shouldStop)
{
    LOG() << "\n >> CCpuLoadStatus started";
    int attempts = 3;
    bool initSuccess = false;
    long lastCpuLoadValue = 0;
   
    while (shouldStop.wait_for(m_RepeatDelay) == std::future_status::timeout)
    {
        std::ostringstream oss;
        std::optional<long> res = utils::GetCpuLoad();

        if (res)
        {
            if (lastCpuLoadValue != *res)
            {
                oss << "\n >> CPU load changed from " << lastCpuLoadValue << "%"
                    << " to " << *res << "%";
                lastCpuLoadValue = *res;
            }
            if (*res >= m_iCriticalLoadValue)
            {
                oss << "\n >> Warning: CPU load is higher than critical value(" << m_iCriticalLoadValue << "%)";
            }
            LOG() << oss.str();
            oss.clear();
        }
        else
        {
            LOG() << "\n >> Couldn't get CPU load data";
        }
      
    }
    LOG() << "\n >> CCpuLoadStatus stopped";
}
