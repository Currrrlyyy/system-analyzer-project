#include "stdafx.h"
#include "abstract_thread.h"

BaseThread::BaseThread(int repeatDelay): 
	m_bIsRunning(false),
	m_RepeatDelay(repeatDelay)
{
}

BaseThread::~BaseThread()
{
	StopAndWait();
}

void BaseThread::Start()
{
	if (m_bIsRunning)
	{
		return;
	}
	m_bIsRunning = true;
	m_StopPromise = std::promise<void>();
	// Execute service in a new thread
	m_Thread = std::thread(&BaseThread::Execute, this, m_StopPromise.get_future());
}

void BaseThread::StopAndWait()
{
	if (!m_bIsRunning)
	{
		return;
	}
	m_StopPromise.set_value();
	m_Thread.join();
	m_bIsRunning = false;
}
