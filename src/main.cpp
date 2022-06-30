#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"
#include "disk_status.h"

int main()
{
	CDiskStatus diskStatus;
	diskStatus.GetDrivesList();
	diskStatus.Start();
	std::cin.get();
	diskStatus.StopAndWait();
}
