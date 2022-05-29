#pragma once
#include "Vectors/Vector3.h"
#include "Matrices/Mat4.h"

namespace AstralEngine
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float w, const Vector3& v);
		Quaternion(float w, float x, float y, float z);

		Quaternion Conjugate() const;
		float Magnitude() const;
		float SqrMagnitude() const;

		void Normalize();
		Quaternion Inverse() const;

		Mat4 GetRotationMatrix() const;
		Vector3 AsEuler() const;
		
		static Quaternion Identity();
		static Quaternion FromEuler(const Vector3& euler);
		static Quaternion FromEuler(float x, float y, float z);

		static Quaternion Normalize(const Quaternion& q);
		static float DotProduct(const Quaternion& q1, const Quaternion& q2);
		static float Angle(const Quaternion& q1, const Quaternion& q2);
		static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
		static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator*(float k) const;
		Quaternion operator/(float k) const;

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;

	private:
		float m_w;
		Vector3 m_v; // axis of rotation
	};

	Quaternion operator*(float k, const Quaternion& q);
}
