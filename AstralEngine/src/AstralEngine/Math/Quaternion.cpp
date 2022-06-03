#include "aepch.h"
#include "Quaternions.h"

namespace AstralEngine
{
	Quaternion::Quaternion() : m_w(0.0f) { }
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

	Mat4 Quaternion::GetRotationMatrix() const
	{
		AE_CORE_WARN("Quaternion::GetRotation can be optimized by caching the multiplications and re-using them");
		
		/*
		return Mat4(
			{ 1.0f - 2.0f * (m_v.y * m_v.y + m_v.z * m_v.z), 2.0f * (m_v.x * m_v.y + m_w * m_v.z),
					2.0f * (m_v.x * m_v.z - m_w * m_v.y), 0.0f },
			{ 2.0f * (m_v.x * m_v.y - m_w * m_v.z), 1.0f - 2.0f * (m_v.x * m_v.x + m_v.z * m_v.z),
					2.0f * (m_v.y * m_v.z + m_w * m_v.x), 0.0f },
			{ 2.0f * (m_v.x * m_v.z + m_w * m_v.y), 2.0f * (m_v.y * m_v.z - m_w * m_v.x),
					1.0f - 2.0f * (m_v.x * m_v.x + m_v.y * m_v.y), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		*/

		/*
		*/
		return Mat4(
			{ 2.0f * (m_w * m_w + m_v.x * m_v.x) - 1.0f, 2.0f * (m_v.x * m_v.y + m_w * m_v.z), 
					2.0f * (m_v.x * m_v.z - m_w * m_v.y), 0.0f },
			{ 2.0f * (m_v.x * m_v.y - m_w * m_v.z), 2.0f * (m_w * m_w + m_v.y * m_v.y) - 1.0f, 
					2.0f * (m_v.y * m_v.z + m_w * m_v.x), 0.0f },
			{ 2.0f * (m_v.x * m_v.z + m_w * m_v.y), 2.0f * (m_v.y * m_v.z - m_w * m_v.x), 
					2.0f * (m_w * m_w + m_v.z * m_v.z) - 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);

		/*
		return Mat4(
				{ 2.0f * (m_w * m_w + m_v.x * m_v.x) - 1.0f, 2.0f * (m_v.x * m_v.y - m_w * m_v.z), 
						2.0f * (m_v.x * m_v.z + m_w * m_v.y), 0.0f },	
				{ 2.0f * (m_v.x * m_v.y + m_w * m_v.z), 2.0f * (m_w * m_w + m_v.y * m_v.y) - 1.0f, 
						2.0f * (m_v.y * m_v.z - m_w * m_v.x), 0.0f },
				{ 2.0f * (m_v.x * m_v.z - m_w * m_v.y), 2.0f * (m_v.y * m_v.z + m_w * m_v.x), 
						2.0f * (m_w * m_w + m_v.z * m_v.z) - 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f }
			);
		*/
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

		return Normalize(Quaternion(
			cosX * cosY * cosZ + sinX * sinY * sinZ,
			sinX * cosY * cosZ - cosX * sinY * sinZ,
			cosX * sinY * cosZ + sinX * cosY * sinZ,
			cosX * cosY * sinZ - sinX * sinY * cosZ
		));
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

	/* taken from forum:
	Just NOTE: acos(dot) is very not stable from numerical point of view.
	as was said previos, q = q1^-1 * q2 and than angle = 2*atan2(q.vec.length(), q.w)
	*/

	float Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
	{
		Quaternion q = q1.Inverse() * q2;
		return Math::ArcTan2(q.EulerAngles().Magnitude(), q.m_w);
		//return Math::ArcCos( DotProduct(q1, q2) / (q1.Magnitude() * q2.Magnitude()) );
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
	
	//from unity, method has been mentioned on this post-> https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion:
	/*
		public static Vector3 operator *(Quaternion rotation, Vector3 point)
        {
            float num = rotation.x * 2f;
            float num2 = rotation.y * 2f;
            float num3 = rotation.z * 2f;
            float num4 = rotation.x * num;
            float num5 = rotation.y * num2;
            float num6 = rotation.z * num3;
            float num7 = rotation.x * num2;
            float num8 = rotation.x * num3;
            float num9 = rotation.y * num3;
            float num10 = rotation.w * num;
            float num11 = rotation.w * num2;
            float num12 = rotation.w * num3;
            Vector3 result = default(Vector3);
            result.x = (1f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
            result.y = (num7 + num12) * point.x + (1f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
            result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1f - (num4 + num5)) * point.z;
            return result;
        }*/

	Vector3 Quaternion::operator*(const Vector3& v) const
	{
		/*
		return 2.0f * Vector3::DotProduct(m_v, v) * m_v
			+ (m_w * m_w - Vector3::DotProduct(m_v, m_v)) * v
			+ 2.0f * m_w * Vector3::CrossProduct(m_v, v);
		*/
		/*
		Quaternion q = Quaternion(0.0f, v);
		Quaternion result = *this * q * Conjugate();
		return result.m_v;
		*/

		/*
		Vector3 t = 2.0f * Vector3::CrossProduct(m_v, v);
		return v + m_w * t + Vector3::CrossProduct(m_v, t);
		*/

		return v + 2.0f * Vector3::CrossProduct(m_v, (m_w * v + Vector3::CrossProduct(m_v, v)));
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