#include "stdafx.h"
#include "utils.h"
#include "logger.h"
#include "cpu_load_status.h"

// interval for status check-up 


CCpuLoadStatus::CCpuLoadStatus(int criticalLoadValue, int cpuLoadDelay):
    m_bIsRunning(false),
    m_CpuLoadDelay(cpuLoadDelay),
    m_iCriticalLoadValue(criticalLoadValue)
{
}

CCpuLoadStatus::~CCpuLoadStatus()
{
    StopAndWait();
}

// Start service
void CCpuLoadStatus::Start()
{
    if (m_bIsRunning)
    {
        return;
    }
    m_bIsRunning = true;
    m_StopPromise = std::promise<void>();
    // Execute service in a new thread
    m_Thread = std::thread(&CCpuLoadStatus::Execute, this, m_StopPromise.get_future());
}

// Stop service and wait for thread to finish the job
void CCpuLoadStatus::StopAndWait()
{
    if (!m_bIsRunning)
    {
        return;
    }
    m_StopPromise.set_value();
    m_Thread.join();
    m_bIsRunning = false;
}

//Execute service to check disks space status
void CCpuLoadStatus::Execute(std::future<void> shouldStop)
{

    std::cout << "\n >> CCpuLoadStatus started";
    int attempts = 3;
    bool initSuccess = false;
    long lastCpuLoadValue = 0;
   
    while (shouldStop.wait_for(m_CpuLoadDelay) == std::future_status::timeout)
    {
        std::ostringstream oss;
        do
        {
            initSuccess = utils::OpenQuery();
            --attempts;
        } while (!initSuccess && attempts != 0);

        if(initSuccess)
        { 
            std::optional<long> res(utils::CollectQueryData());
            if(res)
            {
                if (lastCpuLoadValue != *res)
                {
                    oss << "\n >> CPU load changed from " << lastCpuLoadValue << "%"
                        << " to " << *res << "%";
                    lastCpuLoadValue = *res;
                } 
                if (*res >= m_iCriticalLoadValue)
                    oss << "\n >> Warning: CPU load is higher than critical value(" << m_iCriticalLoadValue << "%)";
            }  
        }
        else
        {
            oss << "\n >> Couldn't get CPU load data";
        }
        LOG() << oss.str();
        oss.clear();

    }

    LOG() << "\n >> CCpuLoadStatus stopped";
}
