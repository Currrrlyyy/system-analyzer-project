#include "stdafx.h"
#include "utils.h"

namespace utils
{
	bool IsConnectedToInternet()
	{
		return InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
	}
} // namespace utils
