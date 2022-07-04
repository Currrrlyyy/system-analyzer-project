#pragma once

class CInternetConnectionStatus
{
public:
	CInternetConnectionStatus(int internetStatusDelay);

	~CInternetConnectionStatus();

	void Start();
	
	void StopAndWait();

private:
	void Execute(std::future<void> shouldStop);

private:
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	std::chrono::seconds m_InternetStatusDelay;
	bool m_bIsRunning;
	bool m_bLastInternetConnection;
};