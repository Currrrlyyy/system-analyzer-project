#include "stdafx.h"
#include "utils.h"
#include "disk_status.h"
#include "logger.h"


const std::chrono::duration g_cDiskStatus_CheckDelay = std::chrono::seconds(15);
const static int coefConvertToMB = 1048576;


CDiskStatus::CDiskStatus(unsigned minimalDeltaMB):
    m_iMinimalDeltaMB(minimalDeltaMB),
    m_bIsRunning(false),
    m_bLastDiskSpaceChanged(false)
{
    utils::InitDrives(m_DrivesInfo, m_lastDiskSpace);
}

CDiskStatus::~CDiskStatus()
{
    StopAndWait();
}

void CDiskStatus::Start()
{
    if (m_bIsRunning)
    {
        return;
    }
    m_bIsRunning = true;
    m_StopPromise = std::promise<void>();
    m_Thread = std::thread(&CDiskStatus::Execute, this, m_StopPromise.get_future());
}

void CDiskStatus::StopAndWait()
{
    if (!m_bIsRunning)
    {
        return;
    }
    m_StopPromise.set_value();
    m_Thread.join();
    m_bIsRunning = false;
}

void CDiskStatus::Execute(std::future<void> shouldStop)
{
    LOG() << "CDiskStatus started";

    GetDrivesFullInfo();

    while (shouldStop.wait_for(g_cDiskStatus_CheckDelay) == std::future_status::timeout)
    {  
       UpdateDrivesInfo();
       GetDrivesStatus();
    }

    GetDrivesFullInfo();

    LOG() << "CDiskStatus stopped";
}

void CDiskStatus::GetDrivesFullInfo()
{
    std::ostringstream oss;
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        
        oss << "\nPhysical disk #" << std::to_string(physicalDiskNumber) << ": " << std::endl;
        for (auto& [logicalDisk, space] : logicalDisks)
        {
           std::string str(logicalDisk.begin(), logicalDisk.end());
           oss << "\tLogical drive " << str << std::endl;
           oss << "\tCapacity: " << std::to_string(space.capacity / coefConvertToMB) << " MB" << std::endl;
           oss << "\tFree: " << (space.free / coefConvertToMB) << " MB" << std::endl;
        }
    }
    LOG() << oss.str();

}

void CDiskStatus::UpdateDrivesInfo()
{
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        for (auto& [logicalDisk, space] : logicalDisks)
        {
            space = std::filesystem::space(logicalDisk);
        }
    }
}

void CDiskStatus::GetDrivesStatus()
{
    std::ostringstream oss;
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        for (auto& [logicalDisk, space] : logicalDisks)
        {
            if ( labs( m_lastDiskSpace[logicalDisk] - space.free)  > 10 * coefConvertToMB)
            {
                oss << "\nFree space on physical disk #" << std::to_string(physicalDiskNumber) << ", logical drive " << logicalDisk << " has changed";
                oss << "\tOld value: " << std::to_string(m_lastDiskSpace[logicalDisk] / coefConvertToMB) << " MB";
                oss << "\tNew value: " << std::to_string(space.free / coefConvertToMB) << " MB";
                m_lastDiskSpace[logicalDisk] = space.free;
                LOG() << oss.str();
                oss.clear();
            }
        }
    }
    
}