#pragma once

class CInternetConnectionStatus : public BaseThread 
{
public:
	CInternetConnectionStatus(int internetStatusDelay);

private:
	void Execute(std::future<void> shouldStop);

private:	
	bool m_bLastInternetConnection;
};