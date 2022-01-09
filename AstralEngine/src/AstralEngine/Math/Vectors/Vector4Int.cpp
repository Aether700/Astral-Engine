#include "aepch.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Vector4Int.h"

namespace AstralEngine
{
	Vector4Int::Vector4Int() : x(0), y(0), z(0), w(0) { }
	Vector4Int::Vector4Int(int X, int Y, int Z, int W) : x(X), y(Y), z(Z), w(W) { }
	Vector4Int::Vector4Int(const Vector2& v2) : x(v2.x), y(v2.y), z(0), w(0) { }
	Vector4Int::Vector4Int(const Vector2Int& v2i) : x(v2i.x), y(v2i.y), z(0), w(0) { }
	Vector4Int::Vector4Int(const Vector3& v3) : x(v3.x), y(v3.y), z(v3.z), w(0) { }
	Vector4Int::Vector4Int(const Vector3Int& v3i) : x(v3i.x), y(v3i.y), z(v3i.z), w(0) { }
	Vector4Int::Vector4Int(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z), w(v4.w) { }

	Vector4Int::~Vector4Int() { }

	const float Vector4Int::Length() const
	{
		return Math::Sqrt((float)((x * x) + (y * y) + (z * z) + (w * w)));
	}

	const Vector4Int Vector4Int::Normalize() const
	{
		return Vector4Int(x / Length(), y / Length(), z / Length(), w / Length()); 
	}


	const Vector4Int Vector4Int::Zero() { return Vector4Int(); }

	const int* Vector4Int::Data() const { return &x; }

	const Vector4Int Vector4Int::operator+(const Vector4Int& v) const { return Vector4Int(x + v.x, y + v.y, z + v.z, w + v.w); }
	const Vector4Int Vector4Int::operator-(const Vector4Int& v) const { return Vector4Int(x - v.x, y - v.y, z - v.z, w - v.w); }
	void Vector4Int::operator+=(const Vector4Int& v) 
	{ 
		*this = *this + v;
	}
	
	void Vector4Int::operator-=(const Vector4Int& v) 
	{
		*this = *this - v;
	}

	const Vector4Int Vector4Int::operator*(int k) const { return Vector4Int(x * k, y * k, z * k, w * k); }
	const Vector4Int Vector4Int::operator/(int k) const { return Vector4Int(x / k, y / k, z / k, w / k); }
	const int Vector4Int::operator[](unsigned int index) const
	{
		switch (index)
		{
			case 0:
				return x;

			case 1:
				return y;

			case 2:
				return z;

			case 3:
				return w;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	int& Vector4Int::operator[](unsigned int index)
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;

		case 2:
			return z;

		case 3:
			return w;
		}

		AE_CORE_ERROR("Invalid Index");
	}

	Vector4Int& Vector4Int::operator=(const Vector4Int& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	Vector4Int operator*(float k, const Vector4Int& v) { return v * k; }

	bool Vector4Int::operator==(const Vector4Int& other) const 
	{
		return x == other.x && y == other.y && z == other.z && w == other.w; 
	}
}