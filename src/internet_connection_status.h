#pragma once

#include "utils.h"

class CInternetConnectionStatus
{
public:
	CInternetConnectionStatus();

	void Start();
	
	void Stop();

	~CInternetConnectionStatus();

private:
	void Execute();

private:
	std::thread m_Thread;
	std::atomic<bool> m_bStoped;
	InternetStatus m_LastStatus;
};