#include "stdafx.h"
#include "disk_status.h"


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