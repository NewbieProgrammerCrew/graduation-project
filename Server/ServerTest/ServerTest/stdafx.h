#pragma once
#define			_USE_MATH_DEFINES
#define			SOME_DISTANCE_THRESHOLD 50
#include		<iostream>

#include		<WS2tcpip.h>
#include		<MSWSock.h>

#include		<math.h>
#include		<cstdlib>

#include		<string>
#include		<array>
#include		<map>
#include		<set>



#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

constexpr int MAX_USER = 10;
constexpr int MAX_ITEM = 10;


enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

