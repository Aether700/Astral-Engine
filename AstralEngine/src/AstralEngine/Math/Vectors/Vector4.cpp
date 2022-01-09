#include "aepch.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Vector4Int.h"

namespace AstralEngine
{
	Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	Vector4::Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { }
	Vector4::Vector4(const Vector2& v2) : x(v2.x), y(v2.y), z(0.0f), w(0.0f) { }
	Vector4::Vector4(const Vector2Int& v2i) : x(v2i.x), y(v2i.y), z(0.0f), w(0.0f) { }
	Vector4::Vector4(const Vector3& v3) : x(v3.x), y(v3.y), z(v3.z), w(0.0f) { }
	Vector4::Vector4(const Vector3Int& v3i) : x(v3i.x), y(v3i.y), z(v3i.z), w(0.0f) { }
	Vector4::Vector4(const Vector4Int& v4i) : x(v4i.x), y(v4i.y), z(v4i.z), w(v4i.w) { }

	Vector4::~Vector4() { }

	const float Vector4::Length() const
	{
		return Math::Sqrt((x * x) + (y * y) + (z * z) + (w * w)); 
	}
	
	const Vector4 Vector4::Normalize() const
	{
		return Vector4(x / Length(), y / Length(), z / Length(), w / Length());
	}

	const float* Vector4::Data() const { return &x; }

	const Vector4 Vector4::Zero() { return Vector4(); }

	const Vector4 Vector4::operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	const Vector4 Vector4::operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	
	void Vector4::operator+=(const Vector4& v) 
	{
		*this = *this + v;
	}

	void Vector4::operator-=(const Vector4& v) 
	{
		*this = *this - v;
	}

	const Vector4 Vector4::operator*(float k) const { return Vector4(x * k, y * k, z * k, w * k); }
	const Vector4 Vector4::operator/(float k) const { return Vector4(x / k, y / k, z / k, w / k); }
	float& Vector4::operator[](unsigned int index)
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

	const float Vector4::operator[](unsigned int index) const
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
	
	Vector4& Vector4::operator=(const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	Vector4 operator*(float k, const Vector4& v) { return v * k; }

	bool Vector4::operator==(const Vector4& other) const 
	{
		return x == other.x && y == other.y && z == other.z && w == other.w; 
	}

}