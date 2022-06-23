#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
int main()
{
	CLogger::Init("../log.txt");
	CConfigParser myData;
	LOG() << "Start.\n" ;
	LOG() << "\n" << myData.GetLogData();
	LOG() << "End.\n\n";

	return 0;
}
