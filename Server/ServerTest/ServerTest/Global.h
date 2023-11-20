#pragma once
#include "SESSION.h"
#include "stdafx.h"
#include <mutex>

extern array<SESSION, MAX_USER> clients;
extern array<SESSION, MAX_ITEM> items;

struct Vector3D {
    float x, y, z;

    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3D operator-(const Vector3D& v) const {
        return Vector3D(x - v.x, y - v.y, z - v.z);
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

