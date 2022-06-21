#include "aepch.h"
#include "Quaternions.h"

namespace AstralEngine
{
	Quaternion::Quaternion() : m_w(1.0f) { }
	Quaternion::Quaternion(float w, const Vector3& v) : m_w(w), m_v(v) { }
	Quaternion::Quaternion(float w, float x, float y, float z) : m_w(w), m_v(x, y, z) {	}

	float Quaternion::GetW() const { return m_w; }
	float Quaternion::GetX() const { return m_v.x; }
	float Quaternion::GetY() const { return m_v.y; }
	float Quaternion::GetZ() const { return m_v.z; }

	void Quaternion::SetW(float w) { m_w = w; }
	void Quaternion::SetX(float x) { m_v.x = x; }
	void Quaternion::SetY(float y) { m_v.y = y; }
	void Quaternion::SetZ(float z) { m_v.z = z; }
	
	void Quaternion::Set(float w, float x, float y, float z)
	{
		m_w = w;
		m_v = Vector3(x, y, z);
	}

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

	Mat3 Quaternion::ComputeRotationMatrix() const
	{		
		float twoX = m_v.x * 2.0f;
		float twoY = m_v.y * 2.0f;
		float twoZ = m_v.z * 2.0f;
		float x2 = m_v.x * twoX;
		float y2 = m_v.y * twoY;
		float z2 = m_v.z * twoZ;
		float xy = m_v.x * twoY;
		float xz = m_v.x * twoZ;
		float yz = m_v.y * twoZ;
		float xw = twoX * m_w;
		float yw = twoY * m_w;
		float zw = twoZ * m_w;

		return Mat3(
			{ 1.0f - y2 - z2, xy + zw, xz - yw },
			{ xy - zw, 1.0f - x2 - z2, yz + xw },
			{ xz + yw, yz - xw, 1.0f - x2 - y2 }
		);
	}

	Vector3 Quaternion::EulerAngles() const
	{
		Vector3 euler;
		float w2 = m_w * m_w;
		float x2 = m_v.x * m_v.x;
		float y2 = m_v.y * m_v.y;
		float z2 = m_v.z * m_v.z;

		float unit = w2 + x2 + y2 + z2;

		float test = m_v.x * m_v.y + m_v.z * m_w;

		float approxHalfMagnitude = 0.499f * unit;

		if (test > approxHalfMagnitude) // singularity at north pole of the unit sphere
		{
			euler.y = Math::RadiansToDegree(2.0f * atan2(m_v.x, m_w));
			euler.z = 90.0f;
			euler.x = 0.0f;
		}
		else if (test < -approxHalfMagnitude) // singularity at south pole of the unit sphere
		{
			euler.y = Math::RadiansToDegree(-2.0f * atan2(m_v.x, m_w));
			euler.z = -90.0f;
			euler.x = 0.0f;
		}
		else
		{
			euler.y = Math::RadiansToDegree(atan2(2.0f * m_v.y * m_w - 2.0f * m_v.x * m_v.z, x2 - y2 - z2 + w2));
			euler.z = Math::RadiansToDegree(Math::ArcSin(2.0f * test / unit));
			euler.x = Math::RadiansToDegree(atan2(2.0f * m_v.x * m_w - 2.0f * m_v.y * m_v.z, -x2 + y2 - z2 + w2));
		}
		return euler;
	}





	
	void Quaternion::SetEulerAngles(const Vector3& euler)
	{
		SetEulerAngles(euler.x, euler.y, euler.z);
	}

	void Quaternion::SetEulerAngles(float x, float y, float z)
	{
		*this = EulerToQuaternion(x, y, z);
	}

	Quaternion Quaternion::Identity()
	{
		return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	Quaternion Quaternion::EulerToQuaternion(const Vector3& euler)
	{
		return EulerToQuaternion(euler.x, euler.y, euler.z);
	}

	Quaternion Quaternion::EulerToQuaternion(float x, float y, float z)
	{
		float halfX = Math::DegreeToRadians(x) * 0.5f;
		float halfY = Math::DegreeToRadians(y) * 0.5f;
		float halfZ = Math::DegreeToRadians(z) * 0.5f;

		float cosX = Math::Cos(halfX);
		float cosY = Math::Cos(halfY);
		float cosZ = Math::Cos(halfZ);

		float sinX = Math::Sin(halfX);
		float sinY = Math::Sin(halfY);
		float sinZ = Math::Sin(halfZ);

		float cXY = cosX * cosY;
		float sXY = sinX * sinY;

		return Normalize(Quaternion(
			cXY * cosZ + sXY * sinZ,
			sinX * cosY * cosZ - cosX * sinY * sinZ,
			cosX * sinY * cosZ + sinX * cosY * sinZ,
			cXY * sinZ - sXY * cosZ
		));
	}

	Quaternion Quaternion::AngleAxisToQuaternion(float angle, const Vector3& axis)
	{
		float halfAngle = Math::DegreeToRadians(angle) * 0.5f;
		Quaternion q = Quaternion(Math::Cos(halfAngle), Math::Sin(halfAngle) * axis);
		return Normalize(q);
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
		float dotProd = DotProduct(q1, q2);
		if (dotProd >= 1.0f)
		{
			return 0.0f;
		}
		return Math::RadiansToDegree(2.0f * Math::ArcCos(dotProd));
	}

	Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t)
	{
		return Normalize(Quaternion(Math::Lerp(a.m_w, b.m_w, t), Vector3::Lerp(a.m_v, b.m_v, t)));
	}

	Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		return Normalize(a + t * (b - a));
	}

	Quaternion Quaternion::LookRotation(const Vector3& lookDir, const Vector3& up)
	{	
		Vector3 forward = Vector3::Normalize(lookDir);
		Vector3 right = Vector3::Normalize(Vector3::CrossProduct(up, forward));
		Vector3 localUp = Vector3::CrossProduct(forward, right);
		float& m00 = right.x;
		float& m01 = right.y;
		float& m02 = right.z;
		float& m10 = localUp.x;
		float& m11 = localUp.y;
		float& m12 = localUp.z;
		float& m20 = forward.x;
		float& m21 = forward.y;
		float& m22 = forward.z;


		float trace = (m00 + m11) + m22;
		Quaternion q;
		if (trace > 0.0f)
		{
			float num = Math::Sqrt(trace + 1.0f);
			q.m_w = num * 0.5f;
			num = 0.5f / num;
			q.m_v.x = (m12 - m21) * num;
			q.m_v.y = (m20 - m02) * num;
			q.m_v.z = (m01 - m10) * num;
		}
		else if ((m00 >= m11) && (m00 >= m22))
		{
			float num7 = Math::Sqrt(((1.0f + m00) - m11) - m22);
			float num4 = 0.5f / num7;
			q.m_v.x = 0.5f * num7;
			q.m_v.y = (m01 + m10) * num4;
			q.m_v.z = (m02 + m20) * num4;
			q.m_w = (m12 - m21) * num4;
		}
		else if (m11 > m22)
		{
			float num6 = Math::Sqrt(((1.0f + m11) - m00) - m22);
			float num3 = 0.5f / num6;
			q.m_v.x = (m10 + m01) * num3;
			q.m_v.y = 0.5f * num6;
			q.m_v.z = (m21 + m12) * num3;
			q.m_w = (m20 - m02) * num3;
		}
		else
		{
			float num5 = Math::Sqrt(((1.0f + m22) - m00) - m11);
			float num2 = 0.5f / num5;
			q.m_v.x = (m20 + m02) * num2;
			q.m_v.y = (m21 + m12) * num2;
			q.m_v.z = 0.5f * num5;
			q.m_w = (m01 - m10) * num2;
		}
		return q;
	}

	Quaternion Quaternion::FromToRotation(const Vector3& from, const Vector3& to)
	{
		if (from == Vector3::Zero() || to == Vector3::Zero())
		{
			return Quaternion::Identity();
		}

		Vector3 axis = Vector3::Normalize(Vector3::CrossProduct(to, from));
		float angle = Vector3::Angle(from, to);
		return AngleAxisToQuaternion(angle, axis);
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
		return Quaternion(
			m_w * q.m_w - m_v.x * q.m_v.x - m_v.y * q.m_v.y - m_v.z * q.m_v.z,
			m_w * q.m_v.x + q.m_w * m_v.x + m_v.y * q.m_v.z - q.m_v.y * m_v.z,
			m_w * q.m_v.y + q.m_w * m_v.y + m_v.z * q.m_v.x - q.m_v.z * m_v.x,
			m_w * q.m_v.z + q.m_w * m_v.z + m_v.x * q.m_v.y - q.m_v.x * m_v.y);
	}

	Quaternion Quaternion::operator*(float k) const
	{
		return Quaternion(m_w * k, m_v * k);
	}

	Vector3 Quaternion::operator*(const Vector3& v) const
	{
		Quaternion vQuat = Quaternion(0.0f, v);
		Quaternion result = *this * vQuat * Conjugate();
		return result.m_v;
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