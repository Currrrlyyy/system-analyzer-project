#pragma once

class CDiskStatus : public BaseThread
{
public:
	CDiskStatus(int minimalDeltaMB, int criticalSpace, int diskStatusDelay);

	void GetDrivesFullInfo();

private:
	void Execute(std::future<void> shouldStop);
	void UpdateDrivesInfo();
	void GetDrivesStatus();


private:
	std::map< int, std::map<std::string, std::filesystem::space_info> > m_DrivesInfo;
	std::map<std::string, uintmax_t> m_lastDiskSpace;
	int m_iCriticalSpace;
	int m_iMinimalDeltaMB;
};

