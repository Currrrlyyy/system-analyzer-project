#include "stdafx.h"
#include "utils.h"


namespace utils
{
    static const DWORD g_cServiceConfigBuffer_MaxBytesSize = 8192UL;

    // Try to get system user name
    std::optional<std::string> GetAccountName()
    {
        char* buf = nullptr;
        size_t szBuffer = 0;
        if (_dupenv_s(&buf, &szBuffer, "USERNAME") == 0 && buf != nullptr)
        {
            return buf;
            free(buf);
        }
        return std::nullopt;
    }

    // Return path of service .exe file
    std::optional<std::string> GetServiceBinaryPath(const std::string& csServiceName)
    {
        std::optional<std::string> binaryPath = std::nullopt;
        SC_HANDLE schSCManager = nullptr;
        SC_HANDLE schService = nullptr;

        do
        {
            schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
            if (schSCManager == nullptr)
            {
                break;
            }

            schService = OpenService(schSCManager, csServiceName.c_str(), SERVICE_ALL_ACCESS);
            if (schService == nullptr)
            {
                break;
            }

            std::vector<BYTE> serviceConfigBuffer(g_cServiceConfigBuffer_MaxBytesSize);
            DWORD dwBytesNeeded = 0UL;
            LPQUERY_SERVICE_CONFIG pServiceConfig = (LPQUERY_SERVICE_CONFIG)&serviceConfigBuffer[0];

            if (QueryServiceConfig (schService, pServiceConfig, serviceConfigBuffer.size(), &dwBytesNeeded) == FALSE)
            {
                break;
            }

            binaryPath = pServiceConfig->lpBinaryPathName;

        } while (false);

        if (schService)
        {
            CloseServiceHandle(schService);
            schService = nullptr;
        }

        if (schSCManager)
        {
            CloseServiceHandle(schSCManager);
            schSCManager = nullptr;
        }

        return binaryPath;
    }

    // Return internet connection status
    bool IsConnectedToInternet()
    {
        return InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    }

    // Return number of physical disk on computer
    std::optional<int> FindDiskNumber(char* drivesList)
    {
        std::string name;
        char volumePathName[MAX_PATH] = { 0 };
        char volumeName[MAX_PATH] = { 0 };
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
    
    // Find all logical drives on computer and write data about their storage
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
    }
    
    std::optional<long> GetCpuLoad()
    {
        PDH_HQUERY hCpuQuery = { 0 };
        PDH_HCOUNTER CpuTotal = { 0 };
        PDH_STATUS status = { 0 };

        status = PdhOpenQuery(NULL, NULL, &hCpuQuery);

        if (ERROR_SUCCESS != status)
        {
            return std::nullopt;
        }

        status = PdhAddCounter(hCpuQuery, _TEXT("\\Processor Information(_Total)\\% Processor Time"), NULL, &CpuTotal);
        if (ERROR_SUCCESS != status)
        {
            return std::nullopt;
        }

        DWORD dwData;
        PDH_FMT_COUNTERVALUE counterVal;      
        status = PdhCollectQueryData(hCpuQuery);
        Sleep(1000);
        status = PdhCollectQueryData(hCpuQuery);

        if (ERROR_SUCCESS != status)
        {
            return std::nullopt;
        }
       
        PdhGetFormattedCounterValue(CpuTotal, PDH_FMT_LONG, &dwData, &counterVal);
        status = counterVal.CStatus;
        if (ERROR_SUCCESS != status)
        {
            return std::nullopt;
        }
        PdhRemoveCounter(CpuTotal);
        PdhCloseQuery(hCpuQuery);

        return counterVal.longValue;
    }

} // namespace utils
