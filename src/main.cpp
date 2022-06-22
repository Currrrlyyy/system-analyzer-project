#include <exception>
#include <ios>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <thread>
#include "json.hpp"
#include < type_traits >


using json = nlohmann::json;



class ConfigData
{
public:
    std::ostringstream oss;

    ConfigData()
    {
        jf = default_j;
        ParseConfigFile();
        if (configExists)
        {
            CompareConfigFiles(default_j, jf);
            CheckValuesValidity();
        }
        InitializeMembers();
        LogServiceData();
    }

    int DiskFreeSpace()
    {
        return jf[paramDiskFreeSpace];
    }

    int CpuLoad()
    {
        return jf[paramCpuLoad];
    }

    bool HasInternetConnection()
    {
        return jf[paramInternetConnection];
    }

    std::string ServiceName()
    {
        return jf[paramServiceName];
    }

    void LogServiceData()
    {
        oss << " >> Service parameters:\n";
        oss << "\tService name: " << ServiceName() << "\n";
        oss << "\tFree disk space: " << DiskFreeSpace() << "%\n";
        oss << "\tCpu loaded: " << CpuLoad() << "%\n";
        oss << "\tInternet connection available: " << HasInternetConnection() << "\n";
    }

private:
    void ParseConfigFile()
    {
        configExists = std::filesystem::exists(configFileName);
        std::ifstream ifs;

        if (!configExists)
        {
            oss << " >> User config doesn't exist.\n";
            oss << " >> Service will use default values.\n";
            return;
        }

        oss << " >> User config exists.\n";
        ifs.open(configFileName);
        if (!ifs.is_open())
        {
            oss << "\tError while trying to open user config.\n";
            oss << " >> Service will use default values.\n";
            return;
        }

        try
        {
            oss << " >> Trying to parse user config.\n";
            jf = json::parse(ifs);
            oss << " >> Successfully parsed user config.\n";
        }
        catch (json::parse_error& ex)
        {
            oss << "\tError while trying to parse user config.\n";
            oss << "\t" << ex.what() << "\n";
            oss << " >> Service will use default values.\n";
        }
    }

    void CompareConfigFiles(const json& defaultConfig, json& userConfig)
    {
        bool isFound = false;
        for (auto user_it = userConfig.begin(); user_it != userConfig.end();)
        {
            isFound = false;
            for (auto def_it = defaultConfig.begin(); def_it != defaultConfig.end(); def_it++)
            {
                if (def_it.key() == user_it.key())
                {
                    isFound = true;
                    break;
                }
            }
            if (!isFound)
            {
                oss << " >> Found a custom parameter in config that won't be used: " << user_it.key() << ", " << user_it.value() << std::endl;
                userConfig.erase(user_it++);

            }
            else
            {
                ++user_it;
            }
        }
    }

    void CheckValuesValidity()
    {

        oss << " >> Checking for validity of value types in user config.\n";

        for (const auto& user_el : jf.items())
        {
            if (user_el.value().type() != default_j[user_el.key()].type())
            {
                oss << "\tParameter has incorrect type of value: " << user_el.key() << ", " << user_el.value() << "; Using default value instead\n";
                user_el.value() = default_j[user_el.key()];
            }

        }
    }

    void InitializeMembers()
    {
        serviceName = (jf.contains(paramServiceName)) ? jf[paramServiceName] : default_j[paramServiceName];
        cpuLoad = (jf.contains(paramCpuLoad)) ? jf[paramCpuLoad] : default_j[paramCpuLoad];
        diskFreeSpace = (jf.contains(paramDiskFreeSpace)) ? jf[paramDiskFreeSpace] : default_j[paramServiceName];
        hasInternetConnection = (jf.contains(paramInternetConnection)) ? jf[paramInternetConnection] : default_j[paramInternetConnection];
    }

private:
    const std::string configFileName = "../config/config.json";
    static const json default_j;
    json jf;
    std::string serviceName;
    int diskFreeSpace;
    int cpuLoad;
    bool hasInternetConnection;
    bool configExists;


private:
    const std::string paramServiceName = "serviceName";
    const std::string paramDiskFreeSpace = "diskFreeSpace";
    const std::string paramCpuLoad = "cpuLoad";
    const std::string paramInternetConnection = "hasInternetConnection";


};

const json ConfigData::default_j = {
        {"serviceName", "JesusIncService"},
        {"diskFreeSpace", 90U},
        {"hasInternetConnection", true},
        {"cpuLoad", 40U}
};



int main()
{
    ConfigData myData;
    std::cout << myData.oss.str();
}
