#include "stdafx.h"
<<<<<<< HEAD
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
=======
#include "config_parser.h"

int main()
{
	std::ostringstream oss;
	std::string fullLogName;
	CConfigParser myConfig;

	myConfig.ParseConfigFile(oss);
	fullLogName = myConfig.GetLogPath() + myConfig.GetLogName();
	
	std::cout << oss.str();
	std::cin.get();
}
>>>>>>> json-config-parser
