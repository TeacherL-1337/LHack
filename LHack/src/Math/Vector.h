#pragma once
#include <cmath>
#include <Windows.h>

struct Matrix4x4
{
	float matrix[4][4];

	float* operator[](int index)
	{
		return matrix[index];
	}
};

class Vector2
{
public:
	constexpr Vector2(
		const float x = 0.f,
		const float y = 0.f) noexcept :
		x(x), y(y) {
	}

	constexpr const Vector2& operator-(const Vector2& other) const noexcept { return Vector2{ x - other.x, y - other.y }; }
	constexpr const Vector2& operator+(const Vector2& other) const noexcept { return Vector2{ x + other.x, y + other.y }; }
	constexpr const Vector2& operator/(const float factor) const noexcept { return Vector2{ x / factor, y / factor }; }
	constexpr const Vector2& operator*(const float factor) const noexcept { return Vector2{ x * factor, y * factor }; }

	const bool IsZero() const { return x == 0.0f && y == 0.0f; }

	float x, y;
};

class Vector3
{
public:
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	constexpr const Vector3& operator-(const Vector3& other) const noexcept { return Vector3{ x - other.x, y - other.y, z - other.z }; }
	constexpr const Vector3& operator+(const Vector3& other) const noexcept { return Vector3{ x + other.x, y + other.y, z + other.z }; }
	constexpr const Vector3& operator/(const float factor) const noexcept { return Vector3{ x / factor, y / factor, z / factor }; }
	constexpr const Vector3& operator*(const float factor) const noexcept { return Vector3{ x * factor, y * factor, z * factor }; }

	const bool IsZero() const { return x == 0.0f && y == 0.0f && z == 0.0f; }

	float x, y, z;
};

static float GetDistance(Vector3 u, Vector3 v)
{
	return std::hypot((u - v).x, (u - v).y, (u - v).z);
}

static void CalcAimAngle(Vector3 src, Vector3 dst, Vector2& out)
{
	Vector3 d = dst - src;
	Vector2 angle;

	float horizontalDistance = std::sqrt(d.x * d.x + d.z * d.z);
	
	angle.x = std::atan2(d.x, d.z); 
	angle.y = std::atan2(d.y, horizontalDistance);

	if (out.x < 0)
	{
		out.x += 360.0f;
	}
}

static bool isPointInCircle(Vector2 point, Vector2 center,float radius)
{
	Vector2 d{};

	d = point - center;

	float distanceSquared = d.x * d.x + d.y * d.y;

	return distanceSquared <= radius * radius;
}
