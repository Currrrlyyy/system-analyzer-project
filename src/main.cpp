#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"
#include "disk_status.h"

int main()
{	
	CLogger::Init("../log.txt");
	CInternetConnectionStatus internetConnection;
	CDiskStatus diskStatus(10);
	internetConnection.Start();
	diskStatus.Start();
	std::cin.get();
	diskStatus.StopAndWait();
	internetConnection.StopAndWait();
}
