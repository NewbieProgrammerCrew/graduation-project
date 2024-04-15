#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <sdkddkver.h>

#include <iostream>
#include <thread>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_queue.h>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>
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
#include "Jelly.h"

using namespace rapidjson;
using namespace std;
using boost::asio::ip::tcp;
struct Vector3D {
	double x, y, z;

	Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
	Vector3D() {}
	Vector3D operator-(const Vector3D& v) const {
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	Vector3D operator+(const Vector3D& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	Vector3D operator*(double scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	double dot(const Vector3D& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	double magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3D normalize() const {
		double m = magnitude();
		return Vector3D(x / m, y / m, z / m);
	}
};
