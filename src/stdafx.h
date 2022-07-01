#pragma once

//Config parser includes
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

// Utils includes
#ifdef _WIN32
	#include <Windows.h>
	#include <WinInet.h>
	#include <fileapi.h>
#endif // _WIN32

//Disk Status includes
#include <vector>
#include <map>
