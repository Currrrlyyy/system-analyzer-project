#include "stdafx.h"
#include "config_parser.h"


// Default config file parameters
const static json s_cDefaultJson = {
        {"logPath", "../"},
        {"logName", "log.txt"},
        {"minimalDiskSpaceDelta", 50U},
        {"diskCriticalSpace", 10U},
        {"cpuCriticalLoad", 90U},
        {"cpuLoadDelay", 30U},
        {"diskStatusDelay", 60U},
        {"internetStatusDelay", 60U}
};

// Names of parameters in config
const static std::string s_csConfigFileName = "../../../config/config.json";
const static std::string s_csParamLogPath = "logPath";
const static std::string s_csParamLogName = "logName";
const static std::string s_csParamMinimalDelta = "minimalDiskSpaceDelta";
const static std::string s_csParamCpuCriticalLoad = "cpuCriticalLoad";
const static std::string s_csParamCpuLoadDelay = "cpuLoadDelay";
const static std::string s_csParamCriticalDiskSpace = "diskCriticalSpace";
const static std::string s_csParamDiskStatusDelay = "diskStatusDelay";
const static std::string s_csParamInternetStatusDelay = "internetStatusDelay";



CConfigParser::CConfigParser()
{
    m_UserJson = s_cDefaultJson;
}

int CConfigParser::GetMinimalDeltaMB()
{
    return m_iMinimalDeltaMB;
}

std::string CConfigParser::GetLogPath()
{
    return m_sLogPath;
}

std::string CConfigParser::GetLogName()
{
    return m_sLogName;
}

int CConfigParser::GetCriticalCpuLoad()
{
    return m_iCriticalCpuLoad;
}

int CConfigParser::GetCriticalDiskSpace()
{
    return m_iCriticalDiskSpace;
}

int CConfigParser::GetDiskStatusDelay()
{
    return m_iDiskStatusDelay;
}

int CConfigParser::GetCpuLoadDelay()
{
    return m_iCpuLoadDelay;
}

int CConfigParser::GetInternetStatusDelay()
{
    return m_iInternetStatusDelay;
}

//Parse config file and logs parse procees info
void CConfigParser::ParseConfigFile(std::ostringstream& oss)
{
    bool m_bConfigExists = std::filesystem::exists(s_csConfigFileName);
    bool m_bConfigParseSuccsessful = false;
    std::ifstream ifs;

    if (!m_bConfigExists)
    {
        oss << "\n >> User config doesn't exist.\n";
        oss << "\n >> Service will use default values.\n";
        return;
    }

    oss << "\n >> User config exists.\n";

    // Try to open config with given path
    ifs.open(s_csConfigFileName);
    if (!ifs.is_open())
    {
        oss << " >> Error while trying to open user config.\n";
        oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
        return;
    }

    // Try to parse user config
    try
    {
        oss << " >> Trying to parse user config.\n";
        m_UserJson = json::parse(ifs);
        oss << " >> Successfully parsed user config.\n";
        m_bConfigParseSuccsessful = true;
    }
    // Catch syntax and other errors in config file
    catch (json::parse_error& ex)
    {
        oss << " >> Error while trying to parse user config.\n";
        oss << "\t" << ex.what() << "\n";
        oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
    }

    if (m_bConfigExists && m_bConfigParseSuccsessful)
    {
        ValidateUserConfig(s_cDefaultJson, m_UserJson, oss);
    }

    // Initialize fields in class with parsed values
    InitializeMembers();

    // Log info 
    oss << "\n >> Service parameters:\n";
    oss << "\tMinimal disk space delta in MB: " << GetMinimalDeltaMB() << "\n";
    oss << "\tLog file path: '" << GetLogPath() << "'" << "\n";
    oss << "\tLog file name: '" << GetLogName() << "'" << "\n";
    oss << "\tCritical CPU Load: " << GetCriticalCpuLoad() << "\n";
    oss << "\tCritical disk space: " << GetCriticalDiskSpace() << "\n";
    oss << "\tCpu load status delay: " << GetCpuLoadDelay() << "\n";
    oss << "\tDisk status delay: " << GetDiskStatusDelay() << "\n";
    oss << "\tInternet status delay: " << GetInternetStatusDelay() << "\n";
}

// Check if information in user config is valid
void CConfigParser::ValidateUserConfig(const json& cDefaultConfig, json& userConfig, std::ostringstream& oss)
{
    bool isFound = false;
    // Check if user config has useless parameteres
    for (auto user_it = userConfig.begin(); user_it != userConfig.end();)
    {
        isFound = false;
        for (auto def_it = cDefaultConfig.begin(); def_it != cDefaultConfig.end(); def_it++)
        {
            if (def_it.key() == user_it.key())
            {
                isFound = true;
                break;
            }
        }
        if (!isFound)
        {
            oss << "\tFound a custom parameter in config that won't be used: " << user_it.key() << ", " << user_it.value() << std::endl;
            userConfig.erase(user_it++);

        }
        else
        {
            ++user_it;
        }
    }
    // Check if types of values in user config are correct
    oss << " >> Checking for validity of value types in user config.\n";

    for (const auto& user_el : m_UserJson.items())
    {
        if (user_el.value().type() != s_cDefaultJson[user_el.key()].type())
        {
            oss << "\tParameter '" << user_el.key() << "' has incorrect type of value : " << user_el.value() << "; Using default value instead\n";
            user_el.value() = s_cDefaultJson[user_el.key()];
        }
    }
}

// Use user config parameter, if contains; Otherwise, use default value
void CConfigParser::InitializeMembers()
{
    m_sLogPath = (m_UserJson.contains(s_csParamLogPath)) ? m_UserJson[s_csParamLogPath] : s_cDefaultJson[s_csParamLogPath];
    m_sLogName = (m_UserJson.contains(s_csParamLogName)) ? m_UserJson[s_csParamLogName] : s_cDefaultJson[s_csParamLogName];
    m_iMinimalDeltaMB = (m_UserJson.contains(s_csParamMinimalDelta)) ? m_UserJson[s_csParamMinimalDelta] : s_cDefaultJson[s_csParamMinimalDelta];
    m_iCriticalCpuLoad = (m_UserJson.contains(s_csParamCpuCriticalLoad)) ? m_UserJson[s_csParamCpuCriticalLoad] : s_cDefaultJson[s_csParamCpuCriticalLoad];
    m_iCriticalDiskSpace = (m_UserJson.contains(s_csParamCriticalDiskSpace)) ? m_UserJson[s_csParamCriticalDiskSpace] : s_cDefaultJson[s_csParamCriticalDiskSpace];
    m_iCpuLoadDelay = (m_UserJson.contains(s_csParamCpuLoadDelay)) ? m_UserJson[s_csParamCpuLoadDelay] : s_cDefaultJson[s_csParamCpuLoadDelay];
    m_iDiskStatusDelay = (m_UserJson.contains(s_csParamDiskStatusDelay)) ? m_UserJson[s_csParamDiskStatusDelay] : s_cDefaultJson[s_csParamDiskStatusDelay];
    m_iInternetStatusDelay = (m_UserJson.contains(s_csParamInternetStatusDelay)) ? m_UserJson[s_csParamInternetStatusDelay] : s_cDefaultJson[s_csParamInternetStatusDelay];


}