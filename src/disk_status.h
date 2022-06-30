#pragma once

class CDiskStatus
{
public:
	CDiskStatus();

	~CDiskStatus();

	void Start();
	void StopAndWait();

	void GetDrivesList();
	void LogDiskData();

private:
	void Execute(std::future<void> shouldStop);

private:
	std::map< int, std::map<std::wstring, std::filesystem::space_info> > m_DrivesInfo;
	std::optional<int> FindDiskNumber(wchar_t* drivesList);
	std::thread m_Thread;
	std::promise<void> m_StopPromise;
	bool m_bIsRunning;
	bool m_bLastDiskSpace;


	
};

