#include "stdafx.h"
#include "utils.h"
#include "logger.h"
#include "abstract_thread.h"
#include "internet_connection_status.h"

// interval for status check-up 
CInternetConnectionStatus::CInternetConnectionStatus(int internetStatusDelay): 
	CBaseThread(internetStatusDelay),
	m_bLastInternetConnection(false)
{	
}

//Execute service to check internet connection status
void CInternetConnectionStatus::Execute(std::future<void> shouldStop)
{
	LOG() << "\n >> CInternetConnectionStatus started";

	// Check with certain period
	while (shouldStop.wait_for(m_RepeatDelay) == std::future_status::timeout)
	{
		const bool bInternetConnection = utils::IsConnectedToInternet();
		if (bInternetConnection != m_bLastInternetConnection)
		{
			if (m_bLastInternetConnection)
			{
				LOG() << "\n >> Internet connection status changed from 'Online' to 'Offline'";
			}
			else
			{
				LOG() << "\n >> Internet connection status changed from 'Offline' to 'Online'";
			}
			m_bLastInternetConnection = bInternetConnection;
		}
	}

	LOG() << "\n >> CInternetConnectionStatus stopped";
}
