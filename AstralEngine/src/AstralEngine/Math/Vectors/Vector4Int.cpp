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
	Vector4Int::Vector4Int(const Vector2& v2) : x((int)v2.x), y((int)v2.y), z(0), w(0) { }
	Vector4Int::Vector4Int(const Vector2Int& v2i) : x(v2i.x), y(v2i.y), z(0), w(0) { }
	Vector4Int::Vector4Int(const Vector3& v3) : x((int)v3.x), y((int)v3.y), z((int)v3.z), w(0) { }
	Vector4Int::Vector4Int(const Vector3Int& v3i) : x(v3i.x), y(v3i.y), z(v3i.z), w(0) { }
	Vector4Int::Vector4Int(const Vector4& v4) : x((int)v4.x), y((int)v4.y), z((int)v4.z), w((int)v4.w) { }

	Vector4Int::~Vector4Int() { }

	const float Vector4Int::Magnitude() const
	{
		return Math::Sqrt(SqrMagnitude());
	}

	const float Vector4Int::SqrMagnitude() const
	{
		return ((float)((x * x) + (y * y) + (z * z) + (w * w)));
	}

	const Vector4Int Vector4Int::Normalize() const
	{
		float len = Magnitude();
		return Vector4Int( (int)((float)x / len), (int)((float)y / len), (int)((float)z / len), (int)((float)w / len) );
	}


	const Vector4Int Vector4Int::Zero() { return Vector4Int(); }

	const int* Vector4Int::Data() const { return &x; }

	const Vector4Int Vector4Int::operator+(const Vector4Int& v) const 
	{ 
		return Vector4Int(x + v.x, y + v.y, z + v.z, w + v.w); 
	}
	
	const Vector4Int Vector4Int::operator-(const Vector4Int& v) const 
	{ 
		return Vector4Int(x - v.x, y - v.y, z - v.z, w - v.w); 
	}
	
	const Vector4Int Vector4Int::operator+=(const Vector4Int& v) const { return *this + v; }
	const Vector4Int Vector4Int::operator-=(const Vector4Int& v) const { return *this - v; }
	
	const Vector4Int Vector4Int::operator*(int k) const { return *this * (float)k; }
	
	const Vector4Int Vector4Int::operator*(float k) const 
	{ 
		return Vector4Int( (int)((float)x * k), (int)((float)y * k), (int)((float)z * k), (int)((float)w * k) );
	}
	
	const Vector4Int Vector4Int::operator/(int k) const { return *this / (float)k; }
	
	const Vector4Int Vector4Int::operator/(float k) const 
	{ 
		return Vector4Int( (int)((float)x / k), (int)((float)y / k), (int)((float)z / k), (int)((float)w / k));
	}
	
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
		return 0;
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
		return x;
	}

	Vector4Int& Vector4Int::operator=(const Vector4Int& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	Vector4Int operator*(float k, const Vector4Int& v) { return (v * k); }

	bool Vector4Int::operator==(const Vector4Int& other) const 
	{
		return x == other.x && y == other.y && z == other.z && w == other.w; 
	}
}