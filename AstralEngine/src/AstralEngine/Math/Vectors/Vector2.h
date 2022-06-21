#pragma once


namespace AstralEngine
{
	class Vector2Int;
	class Vector3;
	class Vector3Int;
	class Vector4;
	class Vector4Int;
	
	class Vector2
	{
	public:
		Vector2();
		Vector2(float X, float Y);
		Vector2(const Vector2Int& v2i);
		Vector2(const Vector3& v3);
		Vector2(const Vector3Int& v3i);
		Vector2(const Vector4& v4);
		Vector2(const Vector4Int& v4i);
		~Vector2();

		const float Magnitude() const;
		const float SqrMagnitude() const;
		void Normalize();

		const float* Data() const;

		static const Vector2 Down();
		static const Vector2 Left();
		static const Vector2 Right();
		static const Vector2 Up();
		static const Vector2 Zero();

		static const Vector2 Normalize(const Vector2& v);
		static const float DotProduct(const Vector2& v1, const Vector2& v2);
		
		//returns a vector with the min x and y value of the two vectors provided
		static Vector2 Min(const Vector2& lhs, const Vector2& rhs);

		//returns a vector with the max x and y value of the two vectors provided
		static Vector2 Max(const Vector2& lhs, const Vector2& rhs);

		//clamps the x and y values respectively using the x and y values of the min/max vectors
		static Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max);
		static float Angle(const Vector2& v1, const Vector2& v2);

		const Vector2 operator-() const;
		const Vector2 operator+(const Vector2& v) const;
		const Vector2 operator-(const Vector2& v) const;
		const Vector2& operator+=(const Vector2& v);
		const Vector2& operator-=(const Vector2& v);
		const Vector2 operator*(float k) const;
		const Vector2 operator/(float k) const;
		const float operator[](unsigned int index) const;
		float& operator[](unsigned int index);

		Vector2& operator=(const Vector2& v);

		bool operator==(const Vector2& other) const;

		float x, y;

	};

	const Vector2 operator*(float k, const Vector2& v);

}