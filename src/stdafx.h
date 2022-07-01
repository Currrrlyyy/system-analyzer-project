#pragma once

//Config parser includes
#include <fstream>
#include <filesystem>
#include "nlohmann_json_parser/json_parser.hpp"

//Logger includes
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>

// InternetConncetionStatus includes
#include <atomic>
#include <future>

#ifdef _WIN32
	#include <Windows.h>
	#include <WinInet.h>
#endif // _WIN32


