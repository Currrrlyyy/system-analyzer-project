#pragma once


using json = nlohmann::json;

class CConfigParser
{
public:
	CConfigParser();
	//int GetDiskFreeSpace();
	//int GetCpuLoad();
	//bool GetHasInternetConnection();
	std::string GetLogPath();
	std::string GetLogName();
	std::string GetServiceName();
	void ParseConfigFile(std::ostringstream& oss);

private:
	void ValidateUserConfig(const json& cDefaultConfig, json& userConfig, std::ostringstream& oss);
	void InitializeMembers();

private:
	json m_UserJson;
	std::string m_sServiceName;
	std::string m_sLogPath;
	std::string m_sLogName;
	//int m_iDiskFreeSpace;
	//int m_iCpuLoad;
	//bool m_bHasInternetConnection;

};