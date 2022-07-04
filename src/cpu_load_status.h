#pragma once
#pragma comment(lib,"pdh.lib")

class CCpuLoadStatus
{
public:
	CCpuLoadStatus(int criticalLoadValue, int cpuLoadDelay);

	~CCpuLoadStatus();

	void Start();
	void StopAndWait();

private:
	void Execute(std::future<void> shouldStop);

private:
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	int m_iCriticalLoadValue;
	std::chrono::seconds m_CpuLoadDelay;
	bool m_bIsRunning;
};

