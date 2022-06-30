#pragma once

class CDiskStatus
{
public:
	void GetDrivesList();
	void LogDiskData();

private:
	std::map< int, std::map<std::wstring, std::filesystem::space_info> > m_DrivesInfo;

private:
	std::optional<int> FindDiskNumber(wchar_t* drivesList);
};