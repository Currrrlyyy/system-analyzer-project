#include "stdafx.h"
#include "internet_connection_status.h"
#include "logger.h"

CInternetConnectionStatus::CInternetConnectionStatus()
	: m_bStoped(true)
	, m_LastStatus(InternetStatus::OFFLINE)
{
}

void CInternetConnectionStatus::Start()
{
	m_bStoped = false;
	m_Thread = std::thread(&CInternetConnectionStatus::Execute, this);
}

void CInternetConnectionStatus::Stop()
{
	m_bStoped = true;
}

void CInternetConnectionStatus::Execute()
{
	using namespace std::chrono_literals;

	LOG() << "CInternetConnectionStatus started";

	while (!m_bStoped)
	{
		InternetStatus currentStatus = GetInternetConnectionStatus();
		if (currentStatus != m_LastStatus)
		{
			if (m_LastStatus == InternetStatus::ONLINE)
			{
				LOG() << "Internet conncetion status changet from Online to Offline";
			}
			else
			{
				LOG() << "Internet conncetion status changet from Offline to Online";
			}
			m_LastStatus = currentStatus;
		}
		std::this_thread::sleep_for(1s);
	}

	LOG() << "CInternetConnectionStatus stoped";
}

CInternetConnectionStatus::~CInternetConnectionStatus()
{
	Stop();
	m_Thread.join();
}
