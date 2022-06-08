#pragma once


namespace AstralEngine
{
	class Vector2;
	class Vector3;
	class Vector3Int;
	class Vector4;
	class Vector4Int;
	class Vector2Short;

	class Vector2Int
	{
	public:
		Vector2Int();
		Vector2Int(int X, int Y);
		Vector2Int(const Vector2& v2);
		Vector2Int(const Vector2Short& v2);
		Vector2Int(const Vector3& v3);
		Vector2Int(const Vector3Int& v3i);
		Vector2Int(const Vector4& v4);
		Vector2Int(const Vector4Int& v4i);

		~Vector2Int();

		const float Magnitude() const;
		const float SqrMagnitude() const;

		const int* Data() const;

		static const Vector2Int Down();
		static const Vector2Int Left();
		static const Vector2Int Right();
		static const Vector2Int Up();
		static const Vector2Int Zero();

		const Vector2Int operator+(const Vector2Int& v) const;
		const Vector2Int operator-(const Vector2Int& v) const;
		const Vector2Int& operator+=(const Vector2Int& v);
		const Vector2Int& operator-=(const Vector2Int& v);
		const Vector2Int operator*(int k) const;
		const Vector2Int operator*(float k) const;
		const Vector2Int operator/(int k) const;
		const Vector2Int operator/(float k) const;
		const int operator[](unsigned int index) const;
		int& operator[](unsigned int index);

		Vector2Int& operator=(const Vector2Int& v);

		bool operator==(const Vector2Int& other) const;
		bool operator!=(const Vector2Int& other) const;

		int x, y;
	};

	const Vector2 operator*(float k, const Vector2& v);

	//uses short instead of int to save on memory usage (used by UI system for long term storage)
	class Vector2Short
	{
	public:
		Vector2Short() : x(0), y(0) { }
		Vector2Short(short _x, short _y) : x(_x), y(_y) { }
		Vector2Short(const Vector2Int& other) :x ((short)other.x), y((short)other.y) { }

		const float Magnitude() const;
		const float SqrMagnitude() const;
		const Vector2Short Normalize() const;

		const short* Data() const;

		static const Vector2Short Down();
		static const Vector2Short Left();
		static const Vector2Short Right();
		static const Vector2Short Up();
		static const Vector2Short Zero();

		const Vector2Short operator+(const Vector2Short& v) const;
		const Vector2Short operator-(const Vector2Short& v) const;
		const Vector2Short operator+=(const Vector2Short& v) const;
		const Vector2Short operator-=(const Vector2Short& v) const;
		const Vector2Short operator*(int k) const;
		const Vector2Short operator/(int k) const;
		const short operator[](unsigned int index) const;

		Vector2Short& operator=(const Vector2Short& v);

		bool operator==(const Vector2Short& other) const;
		bool operator!=(const Vector2Short& other) const;

		short x, y;
	};

}