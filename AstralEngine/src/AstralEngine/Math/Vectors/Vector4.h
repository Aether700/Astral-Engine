#pragma once

namespace AstralEngine
{
	class Vector2;
	class Vector2Int;
	class Vector3;
	class Vector3Int;
	class Vector4Int;

	class Vector4
	{
	public:
		Vector4();
		Vector4(float X, float Y, float Z, float W);
		Vector4(const Vector2& v2);
		Vector4(const Vector2Int& v2i);
		Vector4(const Vector3& v3);
		Vector4(const Vector3Int& v3i);
		Vector4(const Vector4Int& v4i);

		~Vector4();

		const float Magnitude() const;
		const float SqrMagnitude() const;
		void Normalize();

		const float* Data() const;

		static const Vector4 Zero();

		static const Vector4 Normalize(const Vector4& v);

		const Vector4 operator+(const Vector4& v) const;
		const Vector4 operator-(const Vector4& v) const;
		const Vector4 operator+=(const Vector4& v) const;
		const Vector4 operator-=(const Vector4& v) const;
		const Vector4 operator*(float k) const;
		const Vector4 operator/(float k) const;
		float& operator[](unsigned int index);
		const float operator[](unsigned int index) const;

		Vector4& operator=(const Vector4& v);

		bool operator==(const Vector4& other) const;

		union
		{
			float x;
			float r;
		};

		union
		{
			float y;
			float g;
		};

		union
		{
			float z;
			float b;
		};

		union
		{
			float w;
			float a;
		};
	};
	
	Vector4 operator*(float k, const Vector4& v);
}