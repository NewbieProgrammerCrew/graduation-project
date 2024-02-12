#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_queue.h>
#include <boost/asio.hpp>
#include <mutex>
#include <string>

#include "Object.h"

using namespace std;
using boost::asio::ip::tcp;
