#pragma once

class CDiskStatus
{
public:
	CDiskStatus(int minimalDeltaMB);

	~CDiskStatus();

	void Start();
	void StopAndWait();
	void GetDrivesFullInfo();

private:
	void Execute(std::future<void> shouldStop);
	void UpdateDrivesInfo();
	void GetDrivesStatus();


private:
	std::map< int, std::map<std::string, std::filesystem::space_info> > m_DrivesInfo;
	std::map<std::string, uintmax_t> m_lastDiskSpace;
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	int m_iMinimalDeltaMB;
	bool m_bIsRunning;
};
