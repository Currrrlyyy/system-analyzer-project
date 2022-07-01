#include "stdafx.h"
#include "config_parser.h"

const static json s_cDefaultJson = {
        {"serviceName", "JesusIncService"},
        {"diskFreeSpace", 90U},
        {"hasInternetConnection", true},
        {"logPath", "../"},
        {"logName", "log.log"},
        {"cpuLoad", 40U}
};
const static std::string s_csConfigFileName = "../config/config.json";
const static std::string s_csParamServiceName = "serviceName";
const static std::string s_csParamLogPath = "logPath";
const static std::string s_csParamLogName = "logName";

//const static std::string s_csParamDiskFreeSpace = "diskFreeSpace";
//const static std::string s_csParamCpuLoad = "cpuLoad";
//const static std::string s_csParamInternetConnection = "hasInternetConnection";

CConfigParser::CConfigParser()
{
    m_UserJson = s_cDefaultJson;
}

//int CConfigParser::GetDiskFreeSpace()
//{
//    return m_iDiskFreeSpace;
//}

//int CConfigParser::GetCpuLoad()
//{
//    return m_iCpuLoad;
//}

//bool CConfigParser::GetHasInternetConnection()
//{
//    return m_bHasInternetConnection;
//}

std::string CConfigParser::GetServiceName()
{
    return m_sServiceName;
}

std::string CConfigParser::GetLogPath()
{
    return m_sLogPath;
}

std::string CConfigParser::GetLogName()
{
    return m_sLogName;
}

void CConfigParser::ParseConfigFile(std::ostringstream& oss)
{
    bool m_bConfigExists = std::filesystem::exists(s_csConfigFileName);
    bool m_bConfigParseSuccsessful = false;
    std::ifstream ifs;

    if (!m_bConfigExists)
    {
        oss << " >> User config doesn't exist.\n";
        oss << " >> Service will use default values.\n";
        return;
    }

    oss << " >> User config exists.\n";
    ifs.open(s_csConfigFileName);
    if (!ifs.is_open())
    {
        oss << "\tError while trying to open user config.\n";
        oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
        return;
    }

    try
    {
        oss << " >> Trying to parse user config.\n";
        m_UserJson = json::parse(ifs);
        oss << " >> Successfully parsed user config.\n";
        m_bConfigParseSuccsessful = true;
    }
    catch (json::parse_error& ex)
    {
        oss << "\tError while trying to parse user config.\n";
        oss << "\t" << ex.what() << "\n";
        oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
    }

    if (m_bConfigExists && m_bConfigParseSuccsessful)
    {
        ValidateUserConfig(s_cDefaultJson, m_UserJson, oss);
    }
    InitializeMembers();

    oss << " >> Service parameters:\n";
    oss << "\tService name: " << GetServiceName() << "\n";
    oss << "\tLog file path: " << GetLogPath() << "\n";
    oss << "\tLog file name: " << GetLogName() << "\n";
    //oss << "\tFree disk space: " << GetDiskFreeSpace() << "\n";
    //oss << "\tCpu loaded: " << GetCpuLoad() << "\n";
    //oss << "\tInternet connection available: " << GetHasInternetConnection() << "\n";

}

void CConfigParser::ValidateUserConfig(const json& cDefaultConfig, json& userConfig, std::ostringstream& oss)
{
    bool isFound = false;
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
    oss << " >> Checking for validity of value types in user config.\n";

    for (const auto& user_el : m_UserJson.items())
    {
        if (user_el.value().type() != s_cDefaultJson[user_el.key()].type())
        {
            oss << "\tParameter has incorrect type of value: " << user_el.key() << ", " << user_el.value() << "; Using default value instead\n";
            user_el.value() = s_cDefaultJson[user_el.key()];
        }

    }
}

void CConfigParser::InitializeMembers()
{
    m_sServiceName = (m_UserJson.contains(s_csParamServiceName)) ? m_UserJson[s_csParamServiceName] : s_cDefaultJson[s_csParamServiceName];
    m_sLogPath = (m_UserJson.contains(s_csParamLogPath)) ? m_UserJson[s_csParamLogPath] : s_cDefaultJson[s_csParamLogPath];
    m_sLogName = (m_UserJson.contains(s_csParamLogName)) ? m_UserJson[s_csParamLogName] : s_cDefaultJson[s_csParamLogName];

    /* m_iCpuLoad = (m_UserJson.contains(s_csParamCpuLoad)) ? m_UserJson[s_csParamCpuLoad] : s_cDefaultJson[s_csParamCpuLoad];
    m_iDiskFreeSpace = (m_UserJson.contains(s_csParamDiskFreeSpace)) ? m_UserJson[s_csParamDiskFreeSpace] : s_cDefaultJson[s_csParamDiskFreeSpace];
    m_bHasInternetConnection = (m_UserJson.contains(s_csParamInternetConnection)) ? m_UserJson[s_csParamInternetConnection] : s_cDefaultJson[s_csParamInternetConnection];*/
}