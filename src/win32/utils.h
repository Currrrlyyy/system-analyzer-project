#pragma once

enum class InternetStatus
{
	ONLINE,
	OFFLINE
};

InternetStatus GetInternetConnectionStatus();
