#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"
#include "disk_status.h"

int main()
{	
	
	CLogger::Init("../log.txt");
	CDiskStatus diskStatus;
	diskStatus.InitDrives();
	diskStatus.Start();
	std::cin.get();
	diskStatus.StopAndWait();
}
