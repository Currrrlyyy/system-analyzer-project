#include "stdafx.h"
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