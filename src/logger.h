#pragma once

#pragma warning(disable : 4996)

class CLogger
{
public:
	static void Init(const std::string& csLogFileName);

public:
	CLogger();

	~CLogger();

	CLogger& AsLValue();

	template<class T>
	friend CLogger& operator<<(CLogger& logger, const T& cValue)
	{
		logger.m_Buffer << cValue;
		return logger;
	}

private:
	void AddTimestamp();

	void AddAccountName();

private:
	static std::optional<std::string> s_LogFileName;
	static std::mutex s_Mutex;

private:
	std::ostringstream m_Buffer;
};

#define LOG() CLogger().AsLValue()
