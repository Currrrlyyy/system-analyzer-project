#include "stdafx.h"
#include "disk_status.h"
#include "logger.h"
#include <iostream>

const std::chrono::duration g_cDiskStatus_CheckDelay = std::chrono::seconds(5);

CDiskStatus::CDiskStatus():
    m_bIsRunning(false),
    m_bLastDiskSpace(false)
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
    std::cout << "CDiskStatus started\n";
    while (shouldStop.wait_for(g_cDiskStatus_CheckDelay) == std::future_status::timeout)
    {
        LogDiskData();
        std::cout << std::endl;
    }

    std::cout << "CDiskStatus stopped";
}

std::optional<int> CDiskStatus::FindDiskNumber(wchar_t* drivesList)
{
    std::string name;
    wchar_t volumePathName[MAX_PATH];
    wchar_t volumeName[MAX_PATH];
    DWORD bufferLength = MAX_PATH;
    BOOL isSuccessful = FALSE;
    LPCWSTR fileName = drivesList;
    HANDLE hdevice;
    VOLUME_DISK_EXTENTS outBuffer = {};
    DWORD bytesReturned = 0;

    GetVolumePathNameW(fileName, volumePathName, bufferLength);
    GetVolumeNameForVolumeMountPointW(volumePathName, volumeName, bufferLength);

    volumeName[wcslen(volumeName) - 1] = '\0';

    hdevice = CreateFileW(
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

void CDiskStatus::GetDrivesList()
{
    DWORD dwSize = MAX_PATH;
    wchar_t logicalDrives[MAX_PATH] = { 0 };
    std::optional<int> res;
    wchar_t* singleDrive;

    GetLogicalDriveStringsW(dwSize, logicalDrives);

    singleDrive = logicalDrives;
    while (*singleDrive)
    {
        res = FindDiskNumber(singleDrive);
        if (res)
        {
            m_DrivesInfo[*res][singleDrive] = std::filesystem::space(singleDrive);
        }
        singleDrive += wcslen(singleDrive) + 1;
    }
}

void CDiskStatus::LogDiskData()
{
    double coef = 1e+6;
    for (auto& [key, value] : m_DrivesInfo)
    {
        std::wcout << "Physical disk #" << key << ": " << std::endl;
        for (auto& logicalDrive : value)
        {
            std::wcout << "\tLogical drive " << logicalDrive.first << std::endl;
            std::wcout << "\tCapacity: " << logicalDrive.second.capacity / coef << " MB" << std::endl;
            std::wcout << "\tAvailable: " << logicalDrive.second.available / coef << " MB" << std::endl;
            std::wcout << "\tFree: " << logicalDrive.second.free / coef << " MB" << std::endl;
            std::cout << std::endl;
        }
    }
}