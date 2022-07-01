#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"

int main()
{
	CLogger::Init("../log.txt");

	LOG() << "START";

	CInternetConnectionStatus internetConncetionStatus;
	internetConncetionStatus.Start();
	std::cin.get();
	internetConncetionStatus.StopAndWait();

	LOG() << "END\n\n";
}
