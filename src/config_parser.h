#pragma once

using json = nlohmann::json;

class CConfigParser
{
public:
	CConfigParser();
	std::string GetLogPath();
	std::string GetLogName();
	int GetMinimalDeltaMB();
	int GetCriticalCpuLoad();
	int GetCriticalDiskSpace();
	int GetDiskStatusDelay();
	int GetCpuLoadDelay();
	int GetInternetStatusDelay();

	void ParseConfigFile(std::ostringstream& oss);

private:
	void ValidateUserConfig(const json& cDefaultConfig, json& userConfig, std::ostringstream& oss);
	void InitializeMembers();

private:
	json m_UserJson;	
	std::string m_sLogPath;
	std::string m_sLogName;
	unsigned m_iMinimalDeltaMB;
	unsigned m_iCriticalCpuLoad;
	unsigned m_iCriticalDiskSpace;
	int m_iInternetStatusDelay;
	int m_iDiskStatusDelay;
	int m_iCpuLoadDelay;

};