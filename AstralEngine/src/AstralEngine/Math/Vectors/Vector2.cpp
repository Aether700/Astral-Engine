#include "aepch.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Vector4Int.h"

namespace AstralEngine
{
	Vector2::Vector2() : x(0.0f), y(0.0f) { }
	Vector2::Vector2(float X, float Y) : x(X), y(Y) { }
	Vector2::Vector2(const Vector2Int& v2i) : x((float)v2i.x), y((float)v2i.y) { }
	Vector2::Vector2(const Vector3& v3) : x(v3.x), y(v3.y) { }
	Vector2::Vector2(const Vector3Int& v3i) : x((float)v3i.x), y((float)v3i.y) { }
	Vector2::Vector2(const Vector4& v4) : x(v4.x), y(v4.y) { }
	Vector2::Vector2(const Vector4Int& v4i) : x((float)v4i.x), y((float)v4i.y) { }


	Vector2::~Vector2() { }



	const float Vector2::Magnitude() const
	{
		return Math::Sqrt(SqrMagnitude()); 
	}
	
	const float Vector2::SqrMagnitude() const
	{
		return (x * x) + (y * y);
	}

	void Vector2::Normalize()
	{
		float len = Magnitude();
		x = x / len;
		y = y / len;
	}

	const float* Vector2::Data() const
	{
		return &x;
	}

	const Vector2 Vector2::Down() { return Vector2(0.0f, -1.0f); }
	const Vector2 Vector2::Left() { return Vector2(-1.0f, 0.0f); }
	const Vector2 Vector2::Right() { return Vector2(1.0f, 0.0f); }
	const Vector2 Vector2::Up() { return Vector2(0.0f, 1.0f); }
	const Vector2 Vector2::Zero() { return Vector2(); }

	const Vector2 Vector2::Normalize(const Vector2& v)
	{
		Vector2 unit = v;
		unit.Normalize();
		return unit;
	}

	const float Vector2::DotProduct(const Vector2& v1, const Vector2& v2) { return (v1.x * v2.x) + (v1.y * v2.y); }
	Vector2 Vector2::Min(const Vector2& lhs, const Vector2& rhs) 
	{ 
		return Vector2(Math::Min(lhs.x, rhs.x), Math::Min(lhs.y, rhs.y)); 
	}

	Vector2 Vector2::Max(const Vector2& lhs, const Vector2& rhs) 
	{ 
		return Vector2(Math::Max(lhs.x, rhs.x), Math::Max(lhs.y, rhs.y));
	}

	Vector2 Vector2::Clamp(const Vector2& v, const Vector2& min, const Vector2& max) 
	{
		return Vector2(Math::Clamp(v.x, min.x, max.x), Math::Clamp(v.y, min.y, max.y));
	}

	float Vector2::Angle(const Vector2& v1, const Vector2& v2)
	{
		return Math::ArcCos( DotProduct(v1, v2) / (v1.Magnitude() * v2.Magnitude()) );
	}

	float Vector2::Distance(const Vector2& v1, const Vector2& v2)
	{
		return Math::Sqrt(SqrDistance(v1, v2));
	}

	float Vector2::SqrDistance(const Vector2& v1, const Vector2& v2)
	{
		Vector2 distVec = v1 - v2;
		return distVec.SqrMagnitude();
	}


	const Vector2 Vector2::operator-() const { return Vector2(-x, -y); }
	const Vector2 Vector2::operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
	const Vector2 Vector2::operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
	
	const Vector2& Vector2::operator+=(const Vector2& v) 
	{
		*this = *this + v;
		return *this;
	}

	const Vector2& Vector2::operator-=(const Vector2& v) 
	{ 
		*this = *this - v;
		return *this;
	}
	
	const Vector2 Vector2::operator*(float k) const { return Vector2(x * k, y * k); }
	const Vector2 Vector2::operator/(float k) const { return Vector2(x / k, y / k); }
	
	const float Vector2::operator[](unsigned int index) const
	{
		switch(index)
		{
			case 0:
				return x;

			case 1:
				return y;
		}

		AE_CORE_ERROR("Invalid Index");
		return 0.0f;
	}

	float& Vector2::operator[](unsigned int index) 
	{
		switch (index)
		{
		case 0:
			return x;

		case 1:
			return y;
		}

		AE_CORE_ERROR("Invalid Index");
		return x;
	}

	const Vector2 operator*(float k, const Vector2& v) { return v * k; }

	Vector2& Vector2::operator=(const Vector2& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}
}