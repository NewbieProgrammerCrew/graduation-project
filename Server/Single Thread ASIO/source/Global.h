#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define PI 3.14159265358979323846

#include <sdkddkver.h>

#include <iostream>
#include <thread>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_queue.h>
#include <boost/asio.hpp>
#include <mutex>
#include <string>
#include <numbers>
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "rapidjson/document.h"
//#include "stdafx.h"
#include "Object.h"

using namespace rapidjson;
using namespace std;
using boost::asio::ip::tcp;
