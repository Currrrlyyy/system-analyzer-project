#pragma once

#include "stdafx.h"

using json = nlohmann::json;

class CConfigParser
{
public:
	CConfigParser();
	std::string GetLogData();
	int GetDiskFreeSpace();
	int GetCpuLoad();
	bool GetHasInternetConnection();
	std::string GetServiceName();
	void LogServiceData();

private:
	void ParseConfigFile();
	void CompareConfigFiles(const json& cDefaultConfig, json& userConfig);
	void CheckValuesValidity();
	void InitializeMembers();

private:
	std::ostringstream m_Oss;
	json m_UserJson;
	std::string m_sServiceName;
	int m_iDiskFreeSpace;
	int m_iCpuLoad;
	bool m_bHasInternetConnection;
	bool m_bConfigExists;
	bool m_bConfigParseSuccsessful;

private:
	static const std::string s_csParamServiceName;
	static const std::string s_csParamDiskFreeSpace;
	static const std::string s_csParamCpuLoad;
	static const std::string s_csParamInternetConnection;
	static const std::string s_csConfigFileName;
	static const json s_cDefaultJson;
};



