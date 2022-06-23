#include "stdafx.h"
#include "win32/utils.h"

#include <Windows.h>
#include <WinInet.h>


InternetStatus GetInternetConnectionStatus()
{
	if (!InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0))
	{
		return InternetStatus::OFFLINE;
	}
	return InternetStatus::ONLINE;
}
