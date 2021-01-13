#pragma once

namespace AstralEngine
{
	class Vector2;
	class Vector2Int;
	class Vector3;
	class Vector3Int;
	class Vector4;

	class Vector4Int
	{
	public:
		Vector4Int();
		Vector4Int(int X, int Y, int Z, int W);
		Vector4Int(const Vector2& v2);
		Vector4Int(const Vector2Int& v2i);
		Vector4Int(const Vector3& v3);
		Vector4Int(const Vector3Int& v3i);
		Vector4Int(const Vector4& v4i);

		~Vector4Int();

		const float Length() const;
		const Vector4Int Normalize() const;


		static const Vector4Int Zero();

		const int* Data() const;

		const Vector4Int operator+(const Vector4Int& v) const;
		const Vector4Int operator-(const Vector4Int& v) const;
		const Vector4Int operator+=(const Vector4Int& v) const;
		const Vector4Int operator-=(const Vector4Int& v) const;
		const Vector4Int operator*(int k) const;
		const Vector4Int operator/(int k) const;
		const int operator[](unsigned int index) const;

		Vector4Int& operator=(const Vector4Int& v);

		bool operator==(const Vector4Int& other) const;

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

		union
		{
			int w;
			int a;
		};
	};
	
	Vector4Int operator*(float k, const Vector4Int& v);
}