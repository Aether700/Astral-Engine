#include "aepch.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Vector4Int.h"
#include <cmath>

namespace AstralEngine
{
	Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) { }
	Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) { }
	Vector3::Vector3(const Vector2& v2) : x(v2.x), y(v2.y), z(0.0f) { }
	Vector3::Vector3(const Vector2Int& v2i) : x(v2i.x), y(v2i.y), z(0.0f) { }
	Vector3::Vector3(const Vector3Int& v3i) : x(v3i.x), y(v3i.y), z(v3i.z) { }
	Vector3::Vector3(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) { }
	Vector3::Vector3(const Vector4Int& v4i) : x(v4i.x), y(v4i.y), z(v4i.z) { }

	Vector3::~Vector3() { }

	const float Vector3::Length() const
	{
		return Math::Sqrt((x * x) + (y * y) + (z * z)); 
	}

	const Vector3 Vector3::Normalize() const
	{
		return Vector3(x / Length(), y / Length(), z / Length()); 
	}

	const float* Vector3::Data() const { return &x; }

	const Vector3 Vector3::Down() { return Vector3(0.0f, -1.0f, 0.0f); }
	const Vector3 Vector3::Up() { return Vector3(0.0f, 1.0f, 0.0f); }
	const Vector3 Vector3::Left() { return Vector3(-1.0f, 0.0f, 0.0f); }
	const Vector3 Vector3::Right() { return Vector3(1.0f, 0.0f, 0.0f); }
	const Vector3 Vector3::Back() { return Vector3(0.0f, 0.0f, -1.0f); }
	const Vector3 Vector3::Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
	const Vector3 Vector3::Zero() { return Vector3(); }


	const float Vector3::DotProduct(const Vector3& v1, const Vector3& v2) { return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z + v2.z); }
	const Vector3 Vector3::CrossProduct(const Vector3& v1, const Vector3& v2)
	{
		//v1.x  v1.y  v1.z
		//v2.x  v2.y  v2.z

		AE_PROFILE_FUNCTION();
		float xCoord = (v1.y * v2.z) - (v2.y * v1.z);
		float yCoord = (v1.x * v2.z) - (v2.x * v1.z);
		float zCoord = (v1.x * v2.y) - (v2.x * v1.y);

		return Vector3(xCoord, yCoord, zCoord);
	}

	const Vector3 Vector3::operator+(const Vector3& v) const 
	{
		return Vector3(x + v.x, y + v.y, z + v.z); 
	}

	const Vector3 Vector3::operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z); 
	}
	
	void Vector3::operator+=(const Vector3& v)
	{
		*this = *this + v;
	}
	
	void Vector3::operator-=(const Vector3& v)
	{
		*this = *this - v;
	}
	
	const Vector3 Vector3::operator*(float k) const { return Vector3(x * k, y * k, z * k); }
	const Vector3 Vector3::operator/(float k) const { return Vector3(x / k, y / k, z / k); }
	const float Vector3::operator[](unsigned int index) const
	{
		switch (index)
		{
			case 0:
				return x;

			case 1:
				return y;

			case 2:
				return z;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	float& Vector3::operator[](unsigned int index)
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;

		case 2:
			return z;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	Vector3& Vector3::operator=(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	const Vector3 operator*(float k, const Vector3& v) { return v * k; }

	bool Vector3::operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
}