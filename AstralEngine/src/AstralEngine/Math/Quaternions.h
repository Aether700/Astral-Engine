#pragma once
#include "Vectors/Vector3.h"

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
		float DotProduct() const;

		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator*(float k) const;

	private:
		float m_w;
		Vector3 m_v;
	};
}
