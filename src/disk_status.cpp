#include "stdafx.h"
#include "utils.h"
#include "disk_status.h"
#include "logger.h"

// interval for status check-up 
static const std::chrono::duration g_cDiskStatus_CheckDelay = std::chrono::minutes(1);

//Convert megabytes to bytes
static const int g_cCoefBytesToMegaBytes = 1048576;

//Critical space on disk(in %)
static const int g_cCriticalSpaceDiskPercentage = 10;

CDiskStatus::CDiskStatus(int minimalDeltaMB):
    m_iMinimalDeltaMB(minimalDeltaMB),
    m_bIsRunning(false)
{
    utils::InitDrives(m_DrivesInfo, m_lastDiskSpace);
}

CDiskStatus::~CDiskStatus()
{
    StopAndWait();
}

// Start service
void CDiskStatus::Start()
{
    if (m_bIsRunning)
    {
        return;
    }
    m_bIsRunning = true;
    m_StopPromise = std::promise<void>();
    // Execute service in a new thread
    m_Thread = std::thread(&CDiskStatus::Execute, this, m_StopPromise.get_future());
}

// Stop service and wait for thread to finish the job
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

//Execute service to check disks space status
void CDiskStatus::Execute(std::future<void> shouldStop)
{
    LOG() << "\n >> CDiskStatus started";

    GetDrivesFullInfo();

    while (shouldStop.wait_for(g_cDiskStatus_CheckDelay) == std::future_status::timeout)
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
           oss << "\t\tFree: " << (space.free / g_cCoefBytesToMegaBytes) << " MB" << std::endl;
           if ( DriveLowSpace(space) )
           {
               oss << "\t\tWarning, low free space on disk(less than " << g_cCriticalSpaceDiskPercentage << "%)";
           }
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

// Log if free space on disk has changed
void CDiskStatus::GetDrivesStatus()
{
    std::ostringstream oss;
    for (auto& [physicalDiskNumber, logicalDisks] : m_DrivesInfo)
    {
        for (auto& [logicalDisk, space] : logicalDisks)
        {
            if ( labs( m_lastDiskSpace[logicalDisk] - space.free)  > 10 * g_cCoefBytesToMegaBytes)
            {
                oss << "\nFree space on physical disk #" << std::to_string(physicalDiskNumber) << ", logical drive " << logicalDisk << " has changed";
                oss << "\tOld value: " << std::to_string(m_lastDiskSpace[logicalDisk] / g_cCoefBytesToMegaBytes) << " MB";
                oss << "\tNew value: " << std::to_string(space.free / g_cCoefBytesToMegaBytes) << " MB";
                m_lastDiskSpace[logicalDisk] = space.free;
                LOG() << oss.str();
                oss.clear();
            }
        }
    }
    
}

bool CDiskStatus::DriveLowSpace(std::filesystem::space_info driveSpace)
{
    double capacity = driveSpace.capacity;
    double freeSpace = driveSpace.free;
    return (freeSpace * 100 / capacity) < g_cCriticalSpaceDiskPercentage;
}
