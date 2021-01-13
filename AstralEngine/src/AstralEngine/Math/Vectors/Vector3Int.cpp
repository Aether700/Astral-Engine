#include "aepch.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Vector4Int.h"

namespace AstralEngine
{
	Vector3Int::Vector3Int() : x(0), y(0), z(0) { }
	Vector3Int::Vector3Int(int X, int Y, int Z) : x(X), y(Y), z(Z) { }
	Vector3Int::Vector3Int(const Vector2& v2) : x(v2.x), y(v2.y), z(0.0f) { }
	Vector3Int::Vector3Int(const Vector2Int& v2i) : x(v2i.x), y(v2i.y), z(0.0f) { }
	Vector3Int::Vector3Int(const Vector3& v3) : x(v3.x), y(v3.y), z(v3.z) { }
	Vector3Int::Vector3Int(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) { }
	Vector3Int::Vector3Int(const Vector4Int& v4i) : x(v4i.x), y(v4i.y), z(v4i.z) { }

	Vector3Int::~Vector3Int() { }

	const float Vector3Int::Length() const
	{
		return Math::Sqrt((x * x) + (y * y) + (z * z)); 
	}
	
	const Vector3Int Vector3Int::Normalize() const
	{
		return Vector3Int(x / Length(), y / Length(), z / Length()); 
	}

	const int* Vector3Int::Data() const { return &x; }

	const Vector3Int Vector3Int::Down() { return Vector3Int(0, -1, 0); }
	const Vector3Int Vector3Int::Up() { return Vector3Int(0, 1, 0); }
	const Vector3Int Vector3Int::Left() { return Vector3Int(-1, 0, 0); }
	const Vector3Int Vector3Int::Right() { return Vector3Int(1, 0, 0); }
	const Vector3Int Vector3Int::Back() { return Vector3Int(0, 0, -1); }
	const Vector3Int Vector3Int::Forward() { return Vector3Int(0, 0, 1); }
	const Vector3Int Vector3Int::Zero() { return Vector3Int(); }

	const Vector3Int Vector3Int::operator+(const Vector3Int& v) const { return Vector3Int(x + v.x, y + v.y, z + v.z); }
	const Vector3Int Vector3Int::operator-(const Vector3Int& v) const { return Vector3Int(x - v.x, y - v.y, z - v.z); }
	const Vector3Int Vector3Int::operator*(int k) const { return Vector3Int(x * k, y * k, z * k); }
	const Vector3Int Vector3Int::operator/(int k) const { return Vector3Int(x / k, y / k, z / k); }
	const int Vector3Int::operator[](unsigned int index) const
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

	Vector3Int& Vector3Int::operator=(const Vector3Int& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	const Vector3Int Vector3Int::operator+=(const Vector3Int& v) const { return *this + v; }
	const Vector3Int Vector3Int::operator-=(const Vector3Int& v) const { return *this - v; }
	Vector3Int operator*(float k, const Vector3Int& v) { return v * k; }

	bool Vector3Int::operator==(const Vector3Int& other) const 
	{
		return x == other.x && y == other.y && z == other.z; 
	}

}