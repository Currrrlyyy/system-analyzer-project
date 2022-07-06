#include "stdafx.h"
#include "service.h"

int main(int argc, char* argv[])
{	

	if (argc > 1)
	{
		if (argv[1] == std::string("/install"))
		{
			CService::Install();
		}
		else if (argv[1] == std::string("/uninstall"))
		{
			CService::Uninstall();
		}
		else
		{
			std::cout << "Unknown command!" << std::endl;;
		}
		return 0;
	}

	if (!CService::Run())
	{
		std::cout << "Can not run service application as console application" << std::endl;
	}

	return 0;

}
