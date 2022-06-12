#pragma once

namespace AstralEngine
{
	class Vector2;
	class Vector2Int;
	class Vector3Int;
	class Vector4;
	class Vector4Int;

	class Vector3
	{
	public:
		Vector3();
		Vector3(float X, float Y, float Z);
		Vector3(const Vector2& v2);
		Vector3(const Vector2Int& v2i);
		Vector3(const Vector3Int& v3i);
		Vector3(const Vector4& v4);
		Vector3(const Vector4Int& v4i);

		~Vector3();

		const float Magnitude() const;
		const float SqrMagnitude() const;
		void Normalize();

		const float* Data() const;

		static const Vector3 Down();
		static const Vector3 Up();
		static const Vector3 Left();
		static const Vector3 Right();
		static const Vector3 Back();
		static const Vector3 Forward();
		static const Vector3 Zero();

		// orthonormalizes the tangeant using gram schmit based on the normal vector. 
		// If one or both of the vectors is nullptr the provided vectors will be left untouched
		static void OrthoNormalize(Vector3* normal, Vector3* tangeant);
		static const Vector3 Normalize(const Vector3& v);
		static const float DotProduct(const Vector3& v1, const Vector3& v2);
		static const Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);

		// returns the angle between both vectors in degrees
		static float Angle(const Vector3& v1, const Vector3& v2);
		static const Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

		const Vector3 operator-() const;
		const Vector3 operator+(const Vector3& v) const;
		const Vector3 operator-(const Vector3& v) const;
		const Vector3& operator+=(const Vector3& v);
		const Vector3& operator-=(const Vector3& v);
		const Vector3 operator*(float k) const;
		const Vector3 operator/(float k) const;
		const float operator[](unsigned int index) const;
		float& operator[](unsigned int index);

		Vector3& operator=(const Vector3& v);
		

		bool operator==(const Vector3& other) const;

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
	};
	const Vector3 operator*(float k, const Vector3& v);
}