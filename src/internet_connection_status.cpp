#include "stdafx.h"
#include "internet_connection_status.h"
#include "utils.h"
#include "logger.h"

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
	using std::chrono_literals::operator""s;

	LOG() << "CInternetConnectionStatus started";

	while (shouldStop.wait_for(1s) == std::future_status::timeout)
	{
		bool bInternetConnection = utils::IsConnectedToInternet();
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
