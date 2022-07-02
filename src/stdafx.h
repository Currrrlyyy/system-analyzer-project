#pragma once

#include <filesystem>
#include "nlohmann_json_parser/json_parser.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>
#include <atomic>
#include <future>
#include <vector>
#include <map>

#ifdef _WIN32
	#include <Windows.h>
	#include <WinInet.h>
	#include <fileapi.h>
	#include <lmcons.h>
#endif // _WIN32


