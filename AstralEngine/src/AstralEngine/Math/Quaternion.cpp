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
		return Mat4(
			{ 2.0f * (m_w * m_w + m_v.x * m_v.x) - 1.0f, 2.0f * (m_v.x * m_v.y + m_w * m_v.z), 
					2.0f * (m_v.x * m_v.z - m_w * m_v.y), 0.0f },
			{ 2.0f * (m_v.x * m_v.y - m_w * m_v.z), 2.0f * (m_w * m_w + m_v.y * m_v.y) - 1.0f, 
					2.0f * (m_v.y * m_v.z + m_w * m_v.x), 0.0f },
			{ 2.0f * (m_v.x * m_v.z + m_w * m_v.y), 2.0f * (m_v.y * m_v.z - m_w * m_v.x), 
					2.0f * (m_w * m_w + m_v.z * m_v.z) - 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		);
		*/

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

		AE_CORE_WARN("Quaternion::EulerToQuaternion can still be optimized by batching into variables repeating computations");

		/*
		return Normalize(Quaternion(
			cosX * cosY * cosZ + sinX * sinY * sinZ,
			sinX * cosY * cosZ - cosX * sinY * sinZ,
			cosX * sinY * cosZ + sinX * cosY * sinZ,
			cosX * cosY * sinZ - sinX * sinY * cosZ
		));
		*/
		return Quaternion(
			cosX * cosY * cosZ + sinX * sinY * sinZ,
			sinX * cosY * cosZ - cosX * sinY * sinZ,
			cosX * sinY * cosZ + sinX * cosY * sinZ,
			cosX * cosY * sinZ - sinX * sinY * cosZ
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

	/* solution from forum: https://gamedev.stackexchange.com/questions/53129/quaternion-look-at-with-up-vector
		void SceneElement::look_at(const mx::Vector3f& target, const mx::Vector3f& up)
		{
		    mx::Vector3f forward_l = mx::normalize(target - position);
		    mx::Vector3f forward_w(1, 0, 0);
		    mx::Vector3f axis  = forward_l % forward_w;
		    float        angle = mx::rad_to_deg(acos(forward_l * forward_w));
		
		    mx::Vector3f third = axis % forward_w;
		    if (third * forward_l < 0)
		    {
		        angle = - angle;
		    }
		    mx::Quaternionf q1 = mx::axis_angle_to_quaternion(angle, axis);
		
		    mx::Vector3f up_l  = mx::transform(q1, mx::normalize(up));
		    mx::Vector3f right = mx::normalize(forward_l % up);
		    mx::Vector3f up_w  = mx::normalize(right % forward_l);
		
		    mx::Vector3f axis2  = up_l % up_w;
		    float        angle2 = mx::rad_to_deg(acos(up_l * up_w));
		
		    mx::Quaternionf q2 = mx::axis_angle_to_quaternion(angle2, axis2);
		
		    orientation = q2 * q1;
		}
	*/

	/* from forum: https://answers.unity.com/questions/467614/what-is-the-source-code-of-quaternionlookrotation.html
		private static Quaternion QuaternionLookRotation(Vector3 forward, Vector3 up)
		{
		    forward.Normalize();
		
		    Vector3 vector = Vector3.Normalize(forward);
		    Vector3 vector2 = Vector3.Normalize(Vector3.Cross(up, vector));
		    Vector3 vector3 = Vector3.Cross(vector, vector2);
		    var m00 = vector2.x;
		    var m01 = vector2.y;
		    var m02 = vector2.z;
		    var m10 = vector3.x;
		    var m11 = vector3.y;
		    var m12 = vector3.z;
		    var m20 = vector.x;
		    var m21 = vector.y;
		    var m22 = vector.z;
		
		
		    float num8 = (m00 + m11) + m22;
		    var quaternion = new Quaternion();
		    if (num8 > 0f)
		    {
		        var num = (float)Math.Sqrt(num8 + 1f);
		        quaternion.w = num * 0.5f;
		        num = 0.5f / num;
		        quaternion.x = (m12 - m21) * num;
		        quaternion.y = (m20 - m02) * num;
		        quaternion.z = (m01 - m10) * num;
		        return quaternion;
		    }
		    if ((m00 >= m11) && (m00 >= m22))
		    {
		        var num7 = (float)Math.Sqrt(((1f + m00) - m11) - m22);
		        var num4 = 0.5f / num7;
		        quaternion.x = 0.5f * num7;
		        quaternion.y = (m01 + m10) * num4;
		        quaternion.z = (m02 + m20) * num4;
		        quaternion.w = (m12 - m21) * num4;
		        return quaternion;
		    }
		    if (m11 > m22)
		    {
		        var num6 = (float)Math.Sqrt(((1f + m11) - m00) - m22);
		        var num3 = 0.5f / num6;
		        quaternion.x = (m10+ m01) * num3;
		        quaternion.y = 0.5f * num6;
		        quaternion.z = (m21 + m12) * num3;
		        quaternion.w = (m20 - m02) * num3;
		        return quaternion; 
		    }
		    var num5 = (float)Math.Sqrt(((1f + m22) - m00) - m11);
		    var num2 = 0.5f / num5;
		    quaternion.x = (m20 + m02) * num2;
		    quaternion.y = (m21 + m12) * num2;
		    quaternion.z = 0.5f * num5;
		    quaternion.w = (m01 - m10) * num2;
		    return quaternion;
		}
	*/

	Quaternion Quaternion::LookRotation(const Vector3& lookDir, const Vector3& up)
	{
		Vector3 forward = Vector3::Normalize(lookDir);
		Vector3 right = Vector3::Normalize(Vector3::CrossProduct(up, forward));
		Vector3 localUp = Vector3::CrossProduct(forward, right);

		return FromRotationMatrix(Mat3(right, localUp, forward));
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

	Quaternion Quaternion::FromRotationMatrix(const Mat3& rot)
	{	
		Quaternion q;

		// trace is positive so we have a quick and easy conversion
		if (rot.Trace() > 0.0f)
		{
			// compute from w
			float temp = Math::Sqrt(rot.Trace() + 1.0f);
			q.m_w = temp * 0.5f;
			temp = 0.5f / temp;
			q.m_v.x = (rot[1][2] - rot[2][1]) * temp;
			q.m_v.y = (rot[2][0] - rot[0][2]) * temp;
			q.m_v.z = (rot[0][1] - rot[1][0]) * temp;
		}
		else if (rot[0][0] > rot[1][1] && rot[0][0] > rot[2][2]) // m00 is the leading term on the main diagonal
		{
			// compute from x
			float temp = Math::Sqrt(((1.0f + rot[0][0]) - rot[1][1]) - rot[2][2]);
			q.m_v.x = 0.5f * temp;
			temp = 0.5f / temp;
			q.m_w = (rot[1][2] - rot[2][1]) * temp;
			q.m_v.y = (rot[0][1] + rot[1][0]) * temp;
			q.m_v.z = (rot[0][2] + rot[2][0]) * temp;
		}
		else if (rot[1][1] > rot[2][2]) // m11 is the leading term on the main diagonal
		{
			// compute from y
			float temp = Math::Sqrt(((1.0f + rot[1][1]) - rot[0][0]) - rot[2][2]);
			q.m_v.y = 0.5f * temp;
			temp = 0.5f / temp;
			q.m_v.x = (rot[1][0] + rot[0][1]) * temp;
			q.m_v.z = (rot[2][1] + rot[1][2]) * temp;
			q.m_w = (rot[2][0] - rot[0][2]) * temp;
		}
		else // m22 is the leading term on the main diagonal
		{
			float temp = Math::Sqrt(((1.0f + rot[2][2]) - rot[0][0]) - rot[1][1]);
			q.m_v.z = 0.5f * temp;
			temp = 0.5f / temp;
			q.m_v.x = (rot[2][0] + rot[0][2]) * temp;
			q.m_v.y = (rot[2][1] + rot[1][2]) * temp;
			q.m_w = (rot[0][1] - rot[1][0]) * temp;
		}

		return q;
	}

}