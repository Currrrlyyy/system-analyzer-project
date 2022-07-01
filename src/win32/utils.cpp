#include "stdafx.h"
#include "utils.h"

namespace utils
{
	std::optional<std::string> GetAccountName()
	{
		DWORD cbBuffer = UNLEN + 1;
		char szBuffer[UNLEN + 1] = { 0 };
		BOOL bRes = GetUserName(szBuffer, &cbBuffer);
		if (bRes == FALSE)
		{
			return std::nullopt;
		}
		return std::string(szBuffer, static_cast<std::size_t>(cbBuffer));
	}

	bool IsConnectedToInternet()
	{
		return InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0) == TRUE;
	}
} // namespace utils
