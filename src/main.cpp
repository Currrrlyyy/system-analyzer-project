#include <exception>
#include <ios>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <stdexcept>

#include "json.hpp"


using json = nlohmann::json;


class ConfigData
{
public:

    ConfigData()
    {
        std::cout << "Begin!\n";

            if(!ParseConfigFile())
            {
                throw std::runtime_error("Couldn't manage to parse configuration file");
            }
       
        std::cout << "Parsed!\n";

       // InitializeMembers(); 
        std::cout << "Constructed!\n";
    }

    int DiskFreeSpace()
    {
        if(jf.contains("diskFreeSpace"))
        {
            return jf["diskFreeSpace"];
        }
        return default_j["diskFreeSpace"];  
    }

    int CpuLoad()
    {
        if(jf.contains("cpuLoad"))
        {
            return jf["cpuLoad"];
        }
        return default_j["cpuLoad"];    
    }

    bool HasInternetConnection()
    {
        if(jf.contains("hasInternetConnection"))
        {
            return jf["hasInternetConnection"];
        }
        return default_j["hasInternetConnection"];  
    }

    std::string ServiceName()
    {
        if(jf.contains("serviceName"))
        {
            return jf["serviceName"];
        }
        return default_j["serviceName"];
    }

    void ShowLogData()
    {
        std::cout << "Service name: " << ServiceName() << "\n";
        std::cout << "Free disk space: " << DiskFreeSpace() << "%\n";
        std::cout << "Cpu loaded: " << CpuLoad() << "%\n";
        std::cout << "Internet connection available: " << HasInternetConnection() << "\n";
    }
private:
    bool ParseConfigFile()
    {
        bool configExists = std::filesystem::exists(configFileName);
        if(!configExists)
        {
            std::cout << "Config doesn't exist!\n";
            std::cout << "Using default values!\n";
            jf = default_j;
        }
        else 
        {
            std::ifstream ifs(configFileName);
            std::cout << "Config exists!\n";       
            try 
            {
                jf = json::parse(ifs);
            } 
            catch (json::parse_error& ex)
            {
                std::cerr << ex.what() << "\n";
            }
        }
        return true;    
    } 

    void InitializeMembers()
    {
        serviceName = jf["serviceName"];
        diskFreeSpace = jf["diskFreeSpace"];
        cpuLoad = jf["cpuLoad"];
        hasInternetConnection = jf["hasInternetConnection"];
    }

private:
    const std::string configFileName = "../config/config.json";
    static const json default_j;
    json jf;
    std::string serviceName;
    int diskFreeSpace;
    int cpuLoad;
    bool hasInternetConnection;
};

const json ConfigData::default_j = {  
        {"serviceName", "JesusIncService"},
        {"diskFreeSpace", 90},
        {"hasInternetConnection", true},
        {"cpuLoad", 40}
    };


int main()
{
    
    try
    {
        ConfigData myData;
        std::cout << "Service name: ";
        myData.ShowLogData();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
