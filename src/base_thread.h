#pragma once

class CBaseThread
{
public:

	CBaseThread(int iRepeatDelay);
	virtual ~CBaseThread();

	void Start();
	void StopAndWait();

private:
	virtual void Execute(std::future<void> shouldStop) = 0;

protected:
	std::chrono::seconds m_RepeatDelay;

private:
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	bool m_bIsRunning;



};