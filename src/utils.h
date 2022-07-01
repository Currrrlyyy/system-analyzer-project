#pragma once

namespace utils
{
	bool IsConnectedToInternet();

	std::optional<int> FindDiskNumber(char* drivesList);

	void InitDrives(std::map< int, std::map<std::string, std::filesystem::space_info> >& DrivesInfo, std::map<std::string, uintmax_t>& lastDiskSpace);

} // namespace utils
