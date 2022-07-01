#include "stdafx.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"

int main()
{
	std::string fullName;
	std::ostringstream oss;
	CConfigParser configParser;
	configParser.ParseConfigFile(oss);

	fullName = configParser.GetLogPath() + configParser.GetLogName();
	CLogger::Init(fullName);

	LOG() << "START";

	LOG() << "\n" << oss.str();

	LOG() << "END\n\n";
}
