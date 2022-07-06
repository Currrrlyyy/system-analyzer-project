#pragma once
#pragma comment(lib,"pdh.lib")

class CCpuLoadStatus: public CBaseThread
{
public:
	CCpuLoadStatus(int CriticalLoadValue, int iCpuLoadDelay);

private:
	void Execute(std::future<void> shouldStop) override;

private:	
	int m_iCriticalLoadValue;

};