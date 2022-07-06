#include "stdafx.h"
#include "base_thread.h"

CBaseThread::CBaseThread(int iRepeatDelay): 
	m_bIsRunning(false),
	m_RepeatDelay(iRepeatDelay)
{
}

CBaseThread::~CBaseThread()
{
	StopAndWait();
}

void CBaseThread::Start()
{
	if (m_bIsRunning)
	{
		return;
	}
	m_bIsRunning = true;
	m_StopPromise = std::promise<void>();
	m_Thread = std::thread(&CBaseThread::Execute, this, m_StopPromise.get_future());
}

void CBaseThread::StopAndWait()
{
	if (!m_bIsRunning)
	{
		return;
	}
	m_StopPromise.set_value();
	m_Thread.join();
	m_bIsRunning = false;
}
