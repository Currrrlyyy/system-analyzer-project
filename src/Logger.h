#pragma once

#include <chrono>
#include <mutex>
#include <sstream>

class CLogger
{
public:
	static void Init(const std::string& csLogFileName);

public:
	CLogger();

	CLogger& AsLValue();

	~CLogger();

	template<class T>
	friend CLogger& operator<<(CLogger& lh, const T& cValue);

private:
	void AddTimestamp();

private:
	static std::string s_sLogFileName;
	static std::mutex s_Mutex;

private:
	std::ostringstream m_oss;
};

template<class T>
CLogger& operator<<(CLogger& lh, const T& cValue)
{
	lh.m_oss << cValue;
	return lh;
}

#define LOG() CLogger().AsLValue()
