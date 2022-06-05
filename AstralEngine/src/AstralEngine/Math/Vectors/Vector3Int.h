#pragma once

namespace AstralEngine
{
	class Vector2;
	class Vector2Int;
	class Vector3;
	class Vector4;
	class Vector4Int;

	class Vector3Int
	{
	public:
		Vector3Int();
		Vector3Int(int X, int Y, int Z);
		Vector3Int(const Vector2& v2);
		Vector3Int(const Vector2Int& v2i);
		Vector3Int(const Vector3& v3);
		Vector3Int(const Vector4& v4);
		Vector3Int(const Vector4Int& v4i);

		~Vector3Int();

		const float Magnitude() const;
		const float SqrMagnitude() const;

		const int* Data() const;

		static const Vector3Int Down();
		static const Vector3Int Up();
		static const Vector3Int Left();
		static const Vector3Int Right();
		static const Vector3Int Back();
		static const Vector3Int Forward();
		static const Vector3Int Zero();

		const Vector3Int operator+(const Vector3Int& v) const;
		const Vector3Int operator-(const Vector3Int& v) const;
		const Vector3Int& operator+=(const Vector3Int& v);
		const Vector3Int& operator-=(const Vector3Int& v);
		const Vector3Int operator*(int k) const;
		const Vector3Int operator*(float k) const;
		const Vector3Int operator/(int k) const;
		const Vector3Int operator/(float k) const;
		const int operator[](unsigned int index) const;
		int& operator[](unsigned int index);

		Vector3Int& operator=(const Vector3Int& v);

		bool operator==(const Vector3Int& other) const;

		union
		{
			int x;
			int r;
		};

		union
		{
			int y;
			int g;
		};

		union
		{
			int z;
			int b;
		};
	};
	
	Vector3Int operator*(float k, const Vector3Int& v);
}