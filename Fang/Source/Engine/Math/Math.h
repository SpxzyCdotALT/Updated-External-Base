#pragma once

#include <vector>
#include <string>
#include <complex>

namespace SDK
{
    struct Vector2 final {
        float x{ 0.0f }, y{ 0.0f };

        Vector2 operator+(const Vector2& rhs) const {
            return { x + rhs.x, y + rhs.y };
        }
        Vector2 operator-(const Vector2& rhs) const {
            return { x - rhs.x, y - rhs.y };
        }
        Vector2 operator*(float scalar) const {
            return { x * scalar, y * scalar };
        }
        Vector2& operator+=(const Vector2& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
        Vector2& operator-=(const Vector2& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        float distance(const Vector2& other) const {
            float dx = x - other.x;
            float dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    };

    struct Vector3 final {

        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };

        Vector3 operator+(const Vector3& rhs) const {
            return { x + rhs.x, y + rhs.y, z + rhs.z };
        }

        Vector3 operator-(const Vector3& rhs) const {
            return { x - rhs.x, y - rhs.y, z - rhs.z };
        }

        Vector3 operator*(float scalar) const {
            return { x * scalar, y * scalar, z * scalar };
        }

        Vector3 operator*(const Vector3& other) const {
            return { x * other.x, y * other.y, z * other.z };
        }

        Vector3 operator/(const Vector3& rhs) const {
            return { x / rhs.x, y / rhs.y, z / rhs.z };
        }

        Vector3& operator+=(const Vector3& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        bool operator==(const Vector3& other) const {
            return (x == other.x && y == other.y && z == other.z);
        }

        Vector3 operator/(float scalar) const {
            return { x / scalar, y / scalar, z / scalar };
        }

        Vector3 operator-() const {
            return { -x, -y, -z };
        }

        const float magnitude() const {
            return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
        }

        const float distance(Vector3 vector) const {
            return (*this - vector).magnitude();
        }

        const Vector3 normalize() const {
            Vector3 ret;
            float mag = magnitude();
            ret.x = x / mag;
            ret.y = y / mag;
            ret.z = z / mag;
            return ret;
        }

        auto cross(Vector3 vec) const {
            Vector3 ret;
            ret.x = y * vec.z - z * vec.y;
            ret.y = -(x * vec.z - z * vec.x);
            ret.z = x * vec.y - y * vec.x;
            return ret;
        }
    };

    struct Matrix4 final { float data[16]; };

    struct Matrix3 final {
        float data[9];

        Vector3 operator*(const Vector3& v) const {
            return {
                data[0] * v.x + data[1] * v.y + data[2] * v.z,
                data[3] * v.x + data[4] * v.y + data[5] * v.z,
                data[6] * v.x + data[7] * v.y + data[8] * v.z
            };
        }
    };

    struct Vector4 final { float x, y, z, w; };
}