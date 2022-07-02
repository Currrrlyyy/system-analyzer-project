#pragma once

namespace utils
{
	std::optional<std::string> GetAccountName();

	std::optional<std::string> GetServiceBinaryPath(const std::string& csServiceName);

	bool IsConnectedToInternet();

	std::optional<int> FindDiskNumber(char* drivesList);

	void InitDrives(std::map< int, std::map<std::string, std::filesystem::space_info> >& DrivesInfo, std::map<std::string, uintmax_t>& lastDiskSpace);

} // namespace utils
