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
	// Vector2Int //////////////////////////////////////////////////////////////////////////////
	Vector2Int::Vector2Int() : x(0), y(0) { }
	Vector2Int::Vector2Int(int X, int Y) : x(X), y(Y) { }
	Vector2Int::Vector2Int(const Vector2& v2) : x(v2.x), y(v2.y) { }
	Vector2Int::Vector2Int(const Vector2Short& v2) : x(v2.x), y(v2.y) { }
	Vector2Int::Vector2Int(const Vector3& v3) : x(v3.x), y(v3.y) { }
	Vector2Int::Vector2Int(const Vector3Int& v3i) : x(v3i.x), y(v3i.y) { }
	Vector2Int::Vector2Int(const Vector4& v4) : x(v4.x), y(v4.y) { }
	Vector2Int::Vector2Int(const Vector4Int& v4i) : x(v4i.x), y(v4i.y) { }


	Vector2Int::~Vector2Int() { }

	const float Vector2Int::Length() const
	{
		return Math::Sqrt( (x * x) + (y * y) ); 
	}

	const Vector2Int Vector2Int::Normalize() const
	{
		return Vector2Int(x / Length(), y / Length()); 
	}

	const int* Vector2Int::Data() const { return &x; }

	const Vector2Int Vector2Int::Down() { return Vector2Int(0, -1); }
	const Vector2Int Vector2Int::Left() { return Vector2Int(-1, 0); }
	const Vector2Int Vector2Int::Right() { return Vector2Int(1, 0); }
	const Vector2Int Vector2Int::Up() { return Vector2Int(0, 1); }
	const Vector2Int Vector2Int::Zero() { return Vector2Int(); }

	const Vector2Int Vector2Int::operator+(const Vector2Int& v) const { return Vector2Int(x + v.x, y + v.y); }
	const Vector2Int Vector2Int::operator-(const Vector2Int& v) const { return Vector2Int(x - v.x, y - v.y); }
	const Vector2Int Vector2Int::operator*(int k) const { return Vector2Int(x * k, y * k); }
	const Vector2Int Vector2Int::operator/(int k) const { return Vector2Int(x / k, y / k); }
	const int Vector2Int::operator[](unsigned int index) const
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	int& Vector2Int::operator[](unsigned int index)
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	Vector2Int& Vector2Int::operator=(const Vector2Int& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	const Vector2Int Vector2Int::operator+=(const Vector2Int& v) const { return *this + v; }
	const Vector2Int Vector2Int::operator-=(const Vector2Int& v) const { return *this - v; }
	Vector2Int operator*(float k, const Vector2Int& v) { return v * k; }

	bool Vector2Int::operator==(const Vector2Int& other) const { return x == other.x && y == other.y; }
	bool Vector2Int::operator!=(const Vector2Int& other) const { return !(*this == other); }

	// Vector2Short //////////////////////////////////////////////////////////////////////////////
	
	const float Vector2Short::Length() const
	{
		return Math::Sqrt((x * x) + (y * y));
	}

	const Vector2Short Vector2Short::Normalize() const
	{
		return Vector2Short(x / Length(), y / Length());
	}

	const short* Vector2Short::Data() const { return &x; }

	const Vector2Short Vector2Short::Down() { return Vector2Short(0, -1); }
	const Vector2Short Vector2Short::Left() { return Vector2Short(-1, 0); }
	const Vector2Short Vector2Short::Right() { return Vector2Short(1, 0); }
	const Vector2Short Vector2Short::Up() { return Vector2Short(0, 1); }
	const Vector2Short Vector2Short::Zero() { return Vector2Short(); }

	const Vector2Short Vector2Short::operator+(const Vector2Short& v) const { return Vector2Short(x + v.x, y + v.y); }
	const Vector2Short Vector2Short::operator-(const Vector2Short& v) const { return Vector2Short(x - v.x, y - v.y); }
	const Vector2Short Vector2Short::operator*(int k) const { return Vector2Short(x * k, y * k); }
	const Vector2Short Vector2Short::operator/(int k) const { return Vector2Short(x / k, y / k); }
	const short Vector2Short::operator[](unsigned int index) const
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	Vector2Short& Vector2Short::operator=(const Vector2Short& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	bool Vector2Short::operator==(const Vector2Short& other) const { return x == other.x && y == other.y; }
	bool Vector2Short::operator!=(const Vector2Short& other) const { return !(*this == other); }
}