#include "config_parser.h"
#include "stdafx.h"

CConfigParser::CConfigParser()
{
    m_UserJson = s_cDefaultJson;
    ParseConfigFile();
    if (m_bConfigExists && m_bConfigParseSuccsessful)
    {
        CompareConfigFiles(s_cDefaultJson, m_UserJson);
        CheckValuesValidity();
    }
    InitializeMembers();
    LogServiceData();
}

std::string CConfigParser::GetLogData()
{
    return m_Oss.str();
}

int CConfigParser::GetDiskFreeSpace()
{
    return m_iDiskFreeSpace;
}

int CConfigParser::GetCpuLoad()
{
    return m_iCpuLoad;
}

bool CConfigParser::GetHasInternetConnection()
{
    return m_bHasInternetConnection;
}

std::string CConfigParser::GetServiceName()
{
    return m_sServiceName;
}

void CConfigParser::LogServiceData()
{
    m_Oss << " >> Service parameters:\n";
    m_Oss << "\tService name: " << GetServiceName() << "\n";
    m_Oss << "\tFree disk space: " << GetDiskFreeSpace() << "\n";
    m_Oss << "\tCpu loaded: " << GetCpuLoad() << "\n";
    m_Oss << "\tInternet connection available: " << GetHasInternetConnection() << "\n";
}

void CConfigParser::ParseConfigFile()
{
    m_bConfigExists = std::filesystem::exists(s_csConfigFileName);
    std::ifstream ifs;

    if (!m_bConfigExists)
    {
        m_Oss << " >> User config doesn't exist.\n";
        m_Oss << " >> Service will use default values.\n";
        return;
    }

    m_Oss << " >> User config exists.\n";
    ifs.open(s_csConfigFileName);
    if (!ifs.is_open())
    {
        m_Oss << "\tError while trying to open user config.\n";
        m_Oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
        return;
    }

    try
    {
        m_Oss << " >> Trying to parse user config.\n";
        m_UserJson = json::parse(ifs);
        m_Oss << " >> Successfully parsed user config.\n";
        m_bConfigParseSuccsessful = true;
    }
    catch (json::parse_error& ex)
    {
        m_Oss << "\tError while trying to parse user config.\n";
        m_Oss << "\t" << ex.what() << "\n";
        m_Oss << " >> Service will use default values.\n";
        m_bConfigParseSuccsessful = false;
    }
}

void CConfigParser::CompareConfigFiles(const json& cDefaultConfig, json& userConfig)
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
            m_Oss << "\tFound a custom parameter in config that won't be used: " << user_it.key() << ", " << user_it.value() << std::endl;
            userConfig.erase(user_it++);

        }
        else
        {
            ++user_it;
        }
    }
}

void CConfigParser::CheckValuesValidity()
{

    m_Oss << " >> Checking for validity of value types in user config.\n";

    for (const auto& user_el : m_UserJson.items())
    {
        if (user_el.value().type() != s_cDefaultJson[user_el.key()].type())
        {
            m_Oss << "\tParameter has incorrect type of value: " << user_el.key() << ", " << user_el.value() << "; Using default value instead\n";
            user_el.value() = s_cDefaultJson[user_el.key()];
        }

    }
}

void CConfigParser::InitializeMembers()
{
    m_sServiceName = (m_UserJson.contains(s_csParamServiceName)) ? m_UserJson[s_csParamServiceName] : s_cDefaultJson[s_csParamServiceName];
    m_iCpuLoad = (m_UserJson.contains(s_csParamCpuLoad)) ? m_UserJson[s_csParamCpuLoad] : s_cDefaultJson[s_csParamCpuLoad];
    m_iDiskFreeSpace = (m_UserJson.contains(s_csParamDiskFreeSpace)) ? m_UserJson[s_csParamDiskFreeSpace] : s_cDefaultJson[s_csParamDiskFreeSpace];
    m_bHasInternetConnection = (m_UserJson.contains(s_csParamInternetConnection)) ? m_UserJson[s_csParamInternetConnection] : s_cDefaultJson[s_csParamInternetConnection];
}
const json CConfigParser::s_cDefaultJson = {
        {"serviceName", "JesusIncService"},
        {"diskFreeSpace", 90U},
        {"hasInternetConnection", true},
        {"cpuLoad", 40U}
};
const std::string CConfigParser::s_csConfigFileName = "../config/config.json";
const std::string CConfigParser::s_csParamServiceName = "serviceName";
const std::string CConfigParser::s_csParamDiskFreeSpace = "diskFreeSpace";
const std::string CConfigParser::s_csParamCpuLoad = "cpuLoad";
const std::string CConfigParser::s_csParamInternetConnection = "hasInternetConnection";