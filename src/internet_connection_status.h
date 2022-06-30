#pragma once

class CInternetConnectionStatus
{
public:
	CInternetConnectionStatus();

	~CInternetConnectionStatus();

	void Start();
	
	void StopAndWait();

private:
	void Execute(std::future<void> shouldStop);

private:
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	bool m_bIsRunning;
	bool m_bLastInternetConnection;
};