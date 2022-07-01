#pragma once

//Common includes
#include <iostream>
#include <string>

//Config parser includes
#include <fstream>
#include <filesystem>
#include "json_parser.hpp"

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

//Disk Status includes
#include <Windows.h>
#include <fileapi.h>
#include <vector>
#include <filesystem>
#include <map>
#include <optional>
