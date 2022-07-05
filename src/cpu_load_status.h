#pragma once
#pragma comment(lib,"pdh.lib")

class CCpuLoadStatus: public CBaseThread
{
public:
	CCpuLoadStatus(int criticalLoadValue, int cpuLoadDelay);

private:
	void Execute(std::future<void> shouldStop);

private:	
	int m_iCriticalLoadValue;

};