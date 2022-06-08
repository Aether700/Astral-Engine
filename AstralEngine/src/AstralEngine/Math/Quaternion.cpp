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

	Vector3 Quaternion::EulerAngles () const
	{
		Vector3 euler;
		euler.x = Math::RadiantsToDegree(Math::ArcTan2(2.0f * (m_w * m_v.x + m_v.y * m_v.z), 
			1.0f - 2.0f * (m_v.x * m_v.x + m_v.y * m_v.y)));
		
		double yIntermediate = 2.0f * (m_w * m_v.y - m_v.z * m_v.x);
		if (Math::Abs(yIntermediate) >= 1.0)
		{
			euler.y = Math::RadiantsToDegree((float)(Math::Pi() / 2.0 * (Math::Abs(yIntermediate) / yIntermediate)));
		}
		else
		{
			euler.y = Math::RadiantsToDegree(Math::ArcSin((float)yIntermediate));
		}

		euler.z = Math::RadiantsToDegree(Math::ArcTan2(2.0f * (m_w * m_v.z + m_v.x * m_v.y),
			1.0f - 2.0f * (m_v.y * m_v.y + m_v.z * m_v.z)));
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
		float halfX = Math::DegreeToRadiants(x) * 0.5f;
		float halfY = Math::DegreeToRadiants(y) * 0.5f;
		float halfZ = Math::DegreeToRadiants(z) * 0.5f;

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
		float halfAngle = Math::DegreeToRadiants(angle) * 0.5f;
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
		return Math::RadiantsToDegree(2.0f * Math::ArcCos(dotProd));
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
		if (lookDir == Vector3::Zero() || up == Vector3::Zero())
		{
			return Identity();
		}

		Vector3 forward = Vector3::Normalize(lookDir);
		float dot = Vector3::DotProduct(Vector3::Forward(), forward);

		// if the dot product == -1 the look direction is the direct opposite direction to the Vector3::Forward 
		// vector therefore we return a rotation around the world up vector of 180 degrees (pi)
		if (Math::Abs(dot - (-1.0f)) < 0.000001f)
		{
			return Quaternion(Math::Pi(), up);
		}
		else if (Math::Abs(dot - (1.0f)) < 0.000001f) 
		{
			// if the dot product == 1 the forward vector and the look direction are the same therefore
			// we don't need any rotation
			return Quaternion::Identity();
		}

		float angle = Math::ArcCos(dot);
		Vector3 axis = Vector3::CrossProduct(Vector3::Forward(), forward);
		axis = Vector3::Normalize(axis);

		return AngleAxisToQuaternion(Math::RadiantsToDegree(angle), axis);
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
		Mat4 rotationMatrix = ComputeRotationMatrix();
		return (Vector3)(rotationMatrix * Vector4(v.x, v.y, v.z, 1.0f));
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