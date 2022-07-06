#pragma once

class CInternetConnectionStatus : public CBaseThread
{
public:
	CInternetConnectionStatus(int iInternetStatusDelay);

private:
	void Execute(std::future<void> shouldStop);

private:	
	bool m_bLastInternetConnection;
};