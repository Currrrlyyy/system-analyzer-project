#include "stdafx.h"
#include "utils.h"
#include "logger.h"
#include "base_thread.h"
#include "disk_status.h"
 
//Convert megabytes to bytes
static const int g_cCoefBytesToMegaBytes = 1048576;

CDiskStatus::CDiskStatus(int iMinimalDeltaMB, int iCriticalSpace, int iDiskStatusDelay):
    CBaseThread(iMinimalDeltaMB),
    m_iCriticalSpace(iCriticalSpace),
    m_iMinimalDeltaMB(iDiskStatusDelay)
{
}

//Execute service to check disks space status
void CDiskStatus::Execute(std::future<void> shouldStop)
{
    LOG() << "\n >> CDiskStatus started";

    utils::InitDrives(m_DrivesInfo, m_lastDiskSpace);

    GetDrivesFullInfo();

    while (shouldStop.wait_for(m_RepeatDelay) == std::future_status::timeout)
    {  
       UpdateDrivesInfo();
       GetDrivesStatus();
    }

    GetDrivesFullInfo();

    LOG() << "\n >> CDiskStatus stopped";
}

// Log info about space on user disks
void CDiskStatus::GetDrivesFullInfo()
{
    std::ostringstream oss;
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        
        oss << "\n >> Physical disk #" << std::to_string(physicalDiskNumber) << ": ";
        for (auto& [logicalDisk, space] : logicalDisks)
        {
           std::string str(logicalDisk.begin(), logicalDisk.end());
           oss << "\n\t >> Logical drive " << str << std::endl;
           oss << "\t\tCapacity: " << std::to_string(space.capacity / g_cCoefBytesToMegaBytes) << " MB" << std::endl;
           oss << "\t\tFree: " << (space.free / g_cCoefBytesToMegaBytes) << " MB";
        }
    }
    LOG() << oss.str();

}

// Update information about space on disks
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

bool CDiskStatus::isCriticalSpace(std::filesystem::space_info space)
{
    int capacity = space.capacity;
    int freeSpace = space.free;

    return (freeSpace * 100 / capacity) < m_iCriticalSpace;
}

// Log if free space on disk has changed
void CDiskStatus::GetDrivesStatus()
{
    std::ostringstream oss;
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        for (auto& [logicalDisk, space] : logicalDisks)
        {
            if ( labs( m_lastDiskSpace[logicalDisk] - space.free) / g_cCoefBytesToMegaBytes > m_iMinimalDeltaMB )
            {
                oss << "\n\tFree space on physical disk #" << std::to_string(physicalDiskNumber) << ", logical drive " << logicalDisk << " has changed";
                oss << "\tOld value: " << std::to_string(m_lastDiskSpace[logicalDisk] / g_cCoefBytesToMegaBytes) << " MB";
                oss << "\tNew value: " << std::to_string(space.free / g_cCoefBytesToMegaBytes) << " MB";
                if (isCriticalSpace(space))
                {
                    oss << "\n\tWarning: critical value of free space on disk(less than " << m_iCriticalSpace << "%)";
                }
                m_lastDiskSpace[logicalDisk] = space.free;
                LOG() << oss.str();
                oss.clear();
            }
        }
    }
    
}