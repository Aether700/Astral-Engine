#include "aepch.h"
#include "Quaternions.h"

namespace AstralEngine
{
	Quaternion::Quaternion() : m_w(0.0f) { }
	Quaternion::Quaternion(float w, const Vector3& v) : m_w(w), m_v(v) { }
	Quaternion::Quaternion(float w, float x, float y, float z) : m_w(w), m_v(x, y, z) {	}

	Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(m_w, -1.0f * m_v);
	}

	float Quaternion::Magnitude() const
	{
		return Math::Sqrt(SqrMagnitude());
	}

	float Quaternion::SqrMagnitude() const
	{
		return m_w * m_w + m_v.SqrMagnitude();
	}

	void Quaternion::Normalize()
	{
		float magnitude = Magnitude();
		m_w = m_w / magnitude;
		m_v = m_v / magnitude;
	}

	Quaternion Quaternion::Inverse() const
	{
		return Conjugate() / SqrMagnitude();
	}

	Mat4 Quaternion::GetRotationMatrix() const
	{
		return Mat4(
				{ 2.0f * (m_w * m_w + m_v.x * m_v.x) - 1.0f, 2.0f * (m_v.x * m_v.y - m_w * m_v.z), 
						2.0f * (m_v.x * m_v.z + m_w * m_v.y), 0.0f },
				{ 2.0f * (m_v.x * m_v.y + m_w * m_v.z), 2.0f * (m_w * m_w + m_v.y * m_v.y) - 1.0f, 
						2.0f * (m_v.y * m_v.z - m_w * m_v.x), 0.0f },
				{ 2.0f * (m_v.x * m_v.z - m_w * m_v.y), 2.0f * (m_v.y * m_v.z + m_w * m_v.x), 
						2.0f * (m_w * m_w + m_v.z * m_v.z) - 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f }
			);
	}

	/* code to convert to euler:
	EulerAngles ToEulerAngles(Quaternion q) {
		EulerAngles angles;

		// roll (x-axis rotation)
		double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.roll = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
		    angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
		    angles.pitch = std::asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.yaw = std::atan2(siny_cosp, cosy_cosp);

		return angles;
	}
	*/

	Vector3 Quaternion::AsEuler() const
	{
		Vector3 euler;
		euler.x = Math::ArcTan2(2.0f * (m_w + m_v.y * m_v.z), 1.0f - 2.0f * (m_v.x * m_v.x + m_v.y * m_v.y));
		make a copysign function in math?
	}

	Quaternion Quaternion::Identity()
	{
		return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	Quaternion Quaternion::FromEuler(const Vector3& euler)
	{
		return FromEuler(euler.x, euler.y, euler.z);
	}

	Quaternion Quaternion::FromEuler(float x, float y, float z)
	{
		return Quaternion(
			Math::Cos(x) * Math::Cos(y) * Math::Cos(z) + Math::Sin(x) * Math::Sin(y) * Math::Sin(z),
			Math::Sin(x) * Math::Cos(y) * Math::Cos(z) - Math::Cos(x) * Math::Sin(y) * Math::Sin(z),
			Math::Cos(x) * Math::Sin(y) * Math::Cos(z) + Math::Sin(x) * Math::Cos(y) * Math::Sin(z),
			Math::Cos(x) * Math::Cos(y) * Math::Sin(z) - Math::Sin(x) * Math::Sin(y) * Math::Cos(z)
		);
	}

	Quaternion Quaternion::Normalize(const Quaternion& q)
	{
		Quaternion unit = q;
		unit.Normalize();
		return unit;
	}

	float Quaternion::DotProduct(const Quaternion& q1, const Quaternion& q2)
	{
		return q1.m_w * q2.m_w + Vector3::DotProduct(q1.m_v, q2.m_v);
	}

	float Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
	{
		return Math::ArcCos( DotProduct(q1, q2) / (q1.Magnitude() * q2.Magnitude()) );
	}

	Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t)
	{
		return Normalize(Quaternion(Math::Lerp(a.m_w, b.m_w, t), Vector3::Lerp(a.m_v, b.m_v, t)));
	}

	Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		return Normalize(a + t * (b - a));
	}

	Quaternion Quaternion::operator+(const Quaternion& q) const
	{
		return Quaternion(m_w + q.m_w, m_v + q.m_v);
	}

	Quaternion Quaternion::operator-(const Quaternion& q) const
	{
		return Quaternion(m_w - q.m_w, m_v - q.m_v);
	}

	Quaternion Quaternion::operator*(const Quaternion& q) const
	{
		return Quaternion(m_w * q.m_w - Vector3::DotProduct(m_v, q.m_v),
			m_w * q.m_v + q.m_w * m_v + Vector3::CrossProduct(m_v, q.m_v));
	}

	Quaternion Quaternion::operator*(float k) const
	{
		return Quaternion(m_w * k, m_v * k);
	}
	
	Quaternion operator*(float k, const Quaternion& q)
	{
		return q * k;
	}

	Quaternion Quaternion::operator/(float k) const
	{
		return Quaternion(m_w / k, m_v / k);
	}

	bool Quaternion::operator==(const Quaternion& other) const
	{
		return m_w == other.m_w && m_v == other.m_v;
	}

	bool Quaternion::operator!=(const Quaternion& other) const
	{
		return !(*this == other);
	}

}