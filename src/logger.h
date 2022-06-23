#pragma once

#pragma warning(disable : 4996)

class CLogger
{
public:
	static void Init(const std::string& csLogFileName);

public:
	CLogger();

	CLogger& AsLValue();

	~CLogger();

	template<class T>
	friend CLogger& operator<<(CLogger& lh, const T& cValue)
	{
		lh.m_oss << cValue;
		return lh;
	}

private:
	void AddTimestamp();

private:
	static std::optional<std::string> s_sLogFileName;
	static std::mutex s_Mutex;

private:
	std::ostringstream m_oss;
};

#define LOG() CLogger().AsLValue()
