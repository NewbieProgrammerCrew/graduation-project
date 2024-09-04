#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <sdkddkver.h>

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <mutex>
#include <queue>
#include <string>
#include <numbers>
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders; 

#include "rapidjson/document.h"
//#include "stdafx.h"
#include "Object.h"
#include "Jelly.h"
#include "Protocol.h"
#include "lobbyProtocol.h"

using namespace rapidjson;
using namespace std;
using boost::asio::ip::tcp;
constexpr int MAX_ROOM_PLAYER = 2;
struct Vector3D {
	float x, y, z;

	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3D() {}
	Vector3D operator-(const Vector3D& v) const {
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	Vector3D operator+(const Vector3D& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	Vector3D operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	float dot(const Vector3D& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3D normalize() const {
		float m = magnitude();
		return Vector3D(x / m, y / m, z / m);
	}
};
