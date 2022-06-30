#include "stdafx.h"
#include "internet_connection_status.h"
#include "utils.h"
#include "logger.h"

const std::chrono::duration g_cInternetStatus_CheckDelay = std::chrono::minutes(1);

CInternetConnectionStatus::CInternetConnectionStatus()
	: m_bIsRunning(false)
	, m_bLastInternetConnection(false)
{
}

CInternetConnectionStatus::~CInternetConnectionStatus()
{
	StopAndWait();
}

void CInternetConnectionStatus::Start()
{
	if (m_bIsRunning)
	{
		return;
	}
	m_bIsRunning = true;
	m_StopPromise = std::promise<void>();
	m_Thread = std::thread(&CInternetConnectionStatus::Execute, this, m_StopPromise.get_future());
}

void CInternetConnectionStatus::StopAndWait()
{
	if (!m_bIsRunning)
	{
		return;
	}
	m_StopPromise.set_value();
	m_Thread.join();
	m_bIsRunning = false;
}

void CInternetConnectionStatus::Execute(std::future<void> shouldStop)
{
	LOG() << "CInternetConnectionStatus started";

	while (shouldStop.wait_for(g_cInternetStatus_CheckDelay) == std::future_status::timeout)
	{
		const bool bInternetConnection = utils::IsConnectedToInternet();
		if (bInternetConnection != m_bLastInternetConnection)
		{
			if (m_bLastInternetConnection)
			{
				LOG() << "Internet connection status changed from Online to Offline";
			}
			else
			{
				LOG() << "Internet connection status changed from Offline to Online";
			}
			m_bLastInternetConnection = bInternetConnection;
		}
	}

	LOG() << "CInternetConnectionStatus stoped";
}
