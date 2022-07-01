#include "stdafx.h"
#include "utils.h"

namespace utils
{
    bool IsConnectedToInternet()
    {
        return InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    }

    std::optional<int> FindDiskNumber(char* drivesList)
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
    
    void InitDrives(std::map< int, std::map<std::string, std::filesystem::space_info> >& DrivesInfo, std::map<std::string, uintmax_t>& lastDiskSpace)
    {
        DWORD dwSize = MAX_PATH;
        char logicalDrives[MAX_PATH] = { 0 };
        std::optional<int> res;
        char* singleDrive;

        GetLogicalDriveStrings(dwSize, logicalDrives);

        singleDrive = logicalDrives;
        while (*singleDrive != '\0')
        {
            res = FindDiskNumber(singleDrive);
            if (res)
            {
                DrivesInfo[*res][singleDrive] = std::filesystem::space(singleDrive);
                lastDiskSpace.insert(std::pair<std::string, uintmax_t>(singleDrive, std::filesystem::space(singleDrive).free));
            }
            singleDrive += strlen(singleDrive) + 1;

        }
    } // namespace utils
}