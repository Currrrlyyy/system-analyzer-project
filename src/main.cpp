#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"

int main()
{
	CLogger::Init("../log.txt");

	LOG() << "START";
	
	CConfigParser myConfig;
	LOG() << "Parse config file\n" << myConfig.GetLogData();

	{
		CInternetConnectionStatus internetConncetionStatus;
		internetConncetionStatus.Start();
		std::cin.get();
		internetConncetionStatus.Stop();
	}

	LOG() << "END\n\n";
}
