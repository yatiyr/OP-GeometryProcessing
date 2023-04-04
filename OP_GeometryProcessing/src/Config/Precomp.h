#pragma once

#include <random>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <utility>
#include <functional>

#include <time.h>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <filesystem>
#include <fstream>

#include <future>
#include <thread>
#include <atomic>

#include <GeoProcess/System/CoreSystem/Logger.h>

#ifdef OP_PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include <csignal>
#endif