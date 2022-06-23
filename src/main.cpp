#include "stdafx.h"
#include "config_parser.h"
#include "logger.h"

int main()
{
	CConfigParser myConfig;
	CLogger::Init("../log.txt");

	LOG() << "START \n";
	LOG() << "\n" << myConfig.GetLogData();
	LOG() << "END \n\n";
}
