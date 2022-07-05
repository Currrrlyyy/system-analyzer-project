#pragma once

class BaseThread
{
public:

	BaseThread(int repeatDelay);
	~BaseThread();

	void Start();
	void StopAndWait();

protected:
	std::chrono::seconds m_RepeatDelay;

private:
	virtual void Execute(std::future<void> shouldStop) = 0;

private:
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	bool m_bIsRunning;

};