#include "stdafx.h"
#include "disk_status.h"
#include "logger.h"
#include <iostream>

const std::chrono::duration g_cDiskStatus_CheckDelay = std::chrono::seconds(15);
const static int coefConvertToMB = 1048576;


CDiskStatus::CDiskStatus(unsigned minimalDeltaMB):
    m_iMinimalDeltaMB(minimalDeltaMB),
    m_bIsRunning(false),
    m_bLastDiskSpaceChanged(false)
{}

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

std::optional<int> CDiskStatus::FindDiskNumber(char* drivesList)
{
    std::string name;
    char volumePathName[MAX_PATH];
    char volumeName[MAX_PATH];
    DWORD bufferLength = MAX_PATH;
    BOOL isSuccessful = FALSE;
    LPSTR fileName = drivesList;
    HANDLE hdevice;
    VOLUME_DISK_EXTENTS outBuffer = {};
    DWORD bytesReturned = 0;

    GetVolumePathNameA(fileName, volumePathName, bufferLength);
    GetVolumeNameForVolumeMountPointA(volumePathName, volumeName, bufferLength);

    volumeName[strlen(volumeName) - 1] = '\0';

    hdevice = CreateFileA(
        volumeName,          
        FILE_READ_ATTRIBUTES,                
        FILE_SHARE_READ,
        NULL,             
        OPEN_EXISTING,    
        FILE_ATTRIBUTE_NORMAL,                
        NULL);

    isSuccessful = DeviceIoControl(
        hdevice,                       
        IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, 
        NULL,
        0,                       
        &outBuffer,          
        256,
        &bytesReturned,                     
        NULL);

    if (!isSuccessful)
        return std::nullopt;

    return static_cast<int>(outBuffer.Extents[0].DiskNumber);
}

void CDiskStatus::InitDrives()
{
    DWORD dwSize = MAX_PATH;
    char logicalDrives[MAX_PATH] = { 0 };
    std::optional<int> res;
    char* singleDrive;
    
    GetLogicalDriveStrings(dwSize, logicalDrives);

    singleDrive = logicalDrives;
    while (*singleDrive)
    {
        res = FindDiskNumber(singleDrive);
        if (res)
        {
            m_DrivesInfo[*res][singleDrive] = std::filesystem::space(singleDrive);
            m_lastDiskSpace.insert( std::pair<std::string, uintmax_t>( singleDrive, std::filesystem::space(singleDrive).free ) );
        }
        singleDrive += strlen(singleDrive) + 1;
    }
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