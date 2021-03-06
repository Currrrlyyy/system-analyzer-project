#pragma once

class CDiskStatus : public CBaseThread
{
public:
	CDiskStatus(int iMinimalDeltaMB, int iCriticalSpace, int iDiskStatusDelay);
	bool isCriticalSpace(std::filesystem::space_info space);
	
	void GetDrivesFullInfo();

private:
	void Execute(std::future<void> shouldStop) override;

	void UpdateDrivesInfo();
	
	void GetDrivesStatus();

private:
	std::map< int, std::map<std::string, std::filesystem::space_info> > m_DrivesInfo;
	std::map<std::string, uintmax_t> m_lastDiskSpace;
	int m_iCriticalSpace;
	int m_iMinimalDeltaMB;
};

