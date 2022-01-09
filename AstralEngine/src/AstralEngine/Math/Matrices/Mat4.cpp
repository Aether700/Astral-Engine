#include "aepch.h"
#include "AstralEngine/Math/Utils.h"
#include "Mat4.h"
#include "Mat3.h"

namespace AstralEngine
{
	Mat4::Mat4()
	{
		m_vectors[0] = Vector4::Zero();
		m_vectors[1] = Vector4::Zero();
		m_vectors[2] = Vector4::Zero();
		m_vectors[3] = Vector4::Zero();
	}

	Mat4::Mat4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4)
	{
		m_vectors[0] = v1;
		m_vectors[1] = v2;
		m_vectors[2] = v3;
		m_vectors[3] = v4;
	}

	Mat4::Mat4(const Mat4& other)
	{
		m_vectors[0] = other.m_vectors[0];
		m_vectors[1] = other.m_vectors[1];
		m_vectors[2] = other.m_vectors[2];
		m_vectors[3] = other.m_vectors[3];
	}

	Mat4::~Mat4() { }

	float Mat4::Get(int x, int y) const
	{
		return m_vectors[y][x];
	}

	const float* Mat4::Data() const { return &(m_vectors[0].x); }

	float Mat4::Determinant() const
	{
		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		AE_PROFILE_FUNCTION();
		Mat3 i = Mat3(
			{ Get(1, 1), Get(2, 1), Get(3, 1) }, 
			{ Get(1, 2), Get(2, 2), Get(3, 2) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 j = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 2), Get(2, 2), Get(3, 2) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 k = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 1), Get(2, 1), Get(3, 1) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 w = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 1), Get(2, 1), Get(3, 1) },
			{ Get(1, 2), Get(2, 2), Get(3, 2) });

		return (Get(0, 0) * i.Determinant()) - (Get(0, 1) * j.Determinant()) + (Get(0, 2) * k.Determinant())
			- (Get(0, 3) * w.Determinant());
	}

	//returns the zero matrix if matrix is not invertable
	const Mat4 Mat4::Inverse()  const
	{
		AE_PROFILE_FUNCTION();
		float det = Determinant();
		if (det == 0.0f)
		{
			return Mat4::Zero();
		}

		Mat4 adjugate = CalculateAdjugate();
		return adjugate / det;
	}
	
	const Mat4 Mat4::Transpose() const
	{
		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		return Mat4(
			{Get(0, 0), Get(0, 1), Get(0, 2), Get(0, 3)},
			{Get(1, 0), Get(1, 1), Get(1, 2), Get(1, 3)},
			{Get(2, 0), Get(2, 1), Get(2, 2), Get(2, 3)},
			{Get(3, 0), Get(3, 1), Get(3, 2), Get(3, 3)});
	}
	
	const float Mat4::Trace() const
	{
		return Get(0, 0) + Get(1, 1) + Get(2, 2) + Get(3, 3);
	}

	void Mat4::Translate(const Vector3& v)
	{
		*this = *this * Mat4::Translate(Mat4::Identity(), v);
	}

	void Mat4::Rotate(float angle, const Vector3& axis)
	{
		*this = *this * Rotate(Mat4::Identity(), angle, axis);
	}

	void Mat4::Scale(const Vector3& scale)
	{
		*this = *this * Scale(Mat4::Identity(), scale);
	}

	const Mat4 Mat4::Translate(const Mat4& m, const Vector3& v)
	{
		AE_PROFILE_FUNCTION();
		Mat4 result = m;
		result.m_vectors[3] = m.m_vectors[0] * v[0] + m.m_vectors[1] * v[1] + m.m_vectors[2] * v[2] + m.m_vectors[3];
		return result;
	}

	const Mat4 Mat4::Rotate(const Mat4& m, float angle, const Vector3& axis)
	{
		AE_PROFILE_FUNCTION();
		Vector3 normalizedAxis = axis.Normalize();
		float cosVal = Math::Cos(angle);
		float sinVal = Math::Sin(angle);
		Vector3 temp = normalizedAxis * (1.0f - cosVal);

		float f11 = cosVal + temp[0] * axis[0];
		float f12 = temp[1] * axis[0] - sinVal * axis[2];
		float f13 = temp[2] * axis[0] + sinVal * axis[1];

		float f21 = temp[0] * axis[1] + sinVal * axis[2];
		float f22 = cosVal + temp[1] * axis[1];
		float f23 = temp[2] * axis[1] - sinVal * axis[0];

		float f31 = temp[0] * axis[2] - sinVal * axis[1];
		float f32 = temp[1] * axis[2] + sinVal * axis[0];
		float f33 = cosVal + temp[2] * axis[2];

		Mat4 rotation = Mat4(
			{ f11, f21, f31, 0},
			{ f12, f22, f32, 0 },
			{ f13, f23, f33, 0 },
			{ 0, 0, 0, 0 }
		);

		Vector4 v1 = m[0] * rotation[0][0] + m[1] * rotation[0][1] + m[2] * rotation[0][2];
		Vector4 v2 = m[0] * rotation[1][0] + m[1] * rotation[1][1] + m[2] * rotation[1][2];
		Vector4 v3 = m[0] * rotation[2][0] + m[1] * rotation[2][1] + m[2] * rotation[2][2];

		return Mat4(v1, v2, v3, m[3]);
	}

	const Mat4 Mat4::Scale(const Mat4& m, const Vector3& scale)
	{
		AE_PROFILE_FUNCTION();
		return Mat4(m[0] * scale[0], m[1] * scale[1], m[2] * scale[2], m[3]);
	}

	
	const Mat4 Mat4::Ortho(float left, float right, float bottom, float top)
	{
		AE_PROFILE_FUNCTION();
		Mat4 result = Mat4::Identity();
		
		result.m_vectors[0].x = 2.0f / (right - left);
		result.m_vectors[1].y = 2.0f / (top - bottom);
		result.m_vectors[2].z = -1.0f;
		result.m_vectors[3].x = -(right + left) / (right - left);
		result.m_vectors[3].y = -(top + bottom) / (top - bottom);

		return result;
	}

	const Mat4 Mat4::Ortho(float left, float right, float bottom, float top, float nearVal, float farVal)
	{
		AE_PROFILE_FUNCTION();
		Mat4 result = Mat4::Identity();
		result[0][0] = 2.0f / (right - left);
		result[1][1] = 2.0f / (top - bottom);
		result[2][2] = -2.0f / (farVal - nearVal);
		result[3][0] = -(right + left) / (right - left);
		result[3][1] = -(top + bottom) / (top - bottom);
		result[3][2] = -(farVal + nearVal) / (farVal - nearVal);
		return result;
	}

	const Mat4 Mat4::Perspective(float fov, float aspect, float nearVal, float farVal)
	{
		AE_CORE_ASSERT(Math::Abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "");

		float const tanHalfFov = Math::Tan(fov / 2.0f);

		Mat4 result;
		result[0][0] = 1.0f / (aspect * tanHalfFov);
		result[1][1] = 1.0f / (tanHalfFov);
		result[2][2] = -(farVal + nearVal) / (farVal - nearVal);
		result[2][3] = -1.0f;
		result[3][2] = -(2.0f * farVal * nearVal) / (farVal - nearVal);
		return result;
	}

	const Mat4 Mat4::Identity()
	{
		return Mat4(
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f});
	}

	const Mat4 Mat4::Zero() { return Mat4(); }

	const Mat4 Mat4::operator*(float k) const
	{
		return Mat4(
			{ m_vectors[0].x * k,  m_vectors[0].y * k, m_vectors[0].z * k, m_vectors[0].w * k},
			{ m_vectors[1].x * k,  m_vectors[1].y * k, m_vectors[1].z * k, m_vectors[1].w * k},
			{ m_vectors[2].x * k,  m_vectors[2].y * k, m_vectors[2].z * k, m_vectors[2].w * k},
			{ m_vectors[3].x * k,  m_vectors[3].y * k, m_vectors[3].z * k, m_vectors[3].w * k});
	}

	const Mat4 Mat4::operator/(float k) const
	{
		float inverseScalar = 1.0f / k;
		return *this * inverseScalar;
	}

	const Mat4 Mat4::operator*(const Mat4& other) const
	{
		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		AE_PROFILE_FUNCTION();
		float f11 = (Get(0, 0) * other.Get(0, 0)) + (Get(0, 1) * other.Get(1, 0)) + 
			(Get(0, 2) * other.Get(2, 0)) + (Get(0, 3) * other.Get(3, 0));
		float f12 = (Get(0, 0) * other.Get(0, 1)) + (Get(0, 1) * other.Get(1, 1)) + 
			(Get(0, 2) * other.Get(2, 1)) + (Get(0, 3) * other.Get(3, 1));
		float f13 = (Get(0, 0) * other.Get(0, 2)) + (Get(0, 1) * other.Get(1, 2)) + 
			(Get(0, 2) * other.Get(2, 2)) + (Get(0, 3) * other.Get(3, 2));
		float f14 = (Get(0, 0) * other.Get(0, 3)) + (Get(0, 1) * other.Get(1, 3)) + 
			(Get(0, 2) * other.Get(2, 3)) + (Get(0, 3) * other.Get(3, 3));

		float f21 = (Get(1, 0) * other.Get(0, 0)) + (Get(1, 1) * other.Get(1, 0)) + 
			(Get(1, 2) * other.Get(2, 0)) + (Get(1, 3) * other.Get(3, 0));
		float f22 = (Get(1, 0) * other.Get(0, 1)) + (Get(1, 1) * other.Get(1, 1)) + 
			(Get(1, 2) * other.Get(2, 1)) + (Get(1, 3) * other.Get(3, 1));
		float f23 = (Get(1, 0) * other.Get(0, 2)) + (Get(1, 1) * other.Get(1, 2)) + 
			(Get(1, 2) * other.Get(2, 2)) + (Get(1, 3) * other.Get(3, 2));
		float f24 = (Get(1, 0) * other.Get(0, 3)) + (Get(1, 1) * other.Get(1, 3)) + 
			(Get(1, 2) * other.Get(2, 3)) + (Get(1, 3) * other.Get(3, 3));
	
		float f31 = (Get(2, 0) * other.Get(0, 0)) + (Get(2, 1) * other.Get(1, 0)) + 
			(Get(2, 2) * other.Get(2, 0)) + (Get(2, 3) * other.Get(3, 0));
		float f32 = (Get(2, 0) * other.Get(0, 1)) + (Get(2, 1) * other.Get(1, 1)) + 
			(Get(2, 2) * other.Get(2, 1)) + (Get(2, 3) * other.Get(3, 1));
		float f33 = (Get(2, 0) * other.Get(0, 2)) + (Get(2, 1) * other.Get(1, 2)) + 
			(Get(2, 2) * other.Get(2, 2)) + (Get(2, 3) * other.Get(3, 2));
		float f34 = (Get(2, 0) * other.Get(0, 3)) + (Get(2, 1) * other.Get(1, 3)) + 
			(Get(2, 2) * other.Get(2, 3)) + (Get(2, 3) * other.Get(3, 3));

		float f41 = (Get(3, 0) * other.Get(0, 0));
		f41 += (Get(3, 1) * other.Get(1, 0));
		f41 += (Get(3, 2) * other.Get(2, 0));
		f41 += (Get(3, 3) * other.Get(3, 0));
		float f42 = (Get(3, 0) * other.Get(0, 1)) + (Get(3, 1) * other.Get(1, 1)) + 
			(Get(3, 2) * other.Get(2, 1)) + (Get(3, 3) * other.Get(3, 1));
		float f43 = (Get(3, 0) * other.Get(0, 2)) + (Get(3, 1) * other.Get(1, 2)) + 
			(Get(3, 2) * other.Get(2, 2)) + (Get(3, 3) * other.Get(3, 2));
		float f44 = (Get(3, 0) * other.Get(0, 3)) + (Get(3, 1) * other.Get(1, 3)) + 
			(Get(3, 2) * other.Get(2, 3)) + (Get(3, 3) * other.Get(3, 3));

		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		return Mat4(
			{ f11, f21, f31, f41 },
			{ f12, f22, f32, f42 },
			{ f13, f23, f33, f43 },
			{ f14, f24, f34, f44 });
	}

	const Mat4 Mat4::operator+(const Mat4& other) const
	{
		return Mat4(
			{ m_vectors[0].x + other.m_vectors[0].x,
			  m_vectors[0].y + other.m_vectors[0].y,
			  m_vectors[0].z + other.m_vectors[0].z, 
			  m_vectors[0].w + other.m_vectors[0].w },

			{ m_vectors[1].x + other.m_vectors[1].x,
			  m_vectors[1].y + other.m_vectors[1].y,
			  m_vectors[1].z + other.m_vectors[1].z,
          	  m_vectors[1].w + other.m_vectors[1].w },

			{ m_vectors[2].x + other.m_vectors[2].x,
			  m_vectors[2].y + other.m_vectors[2].y,
			  m_vectors[2].z + other.m_vectors[2].z,
			  m_vectors[2].w + other.m_vectors[2].w },
			
			{ m_vectors[3].x + other.m_vectors[3].x,
			  m_vectors[3].y + other.m_vectors[3].y,
			  m_vectors[3].z + other.m_vectors[3].z,
			  m_vectors[3].w + other.m_vectors[3].w });
	}
	
	const Mat4 Mat4::operator-(const Mat4& other) const
	{
		return Mat4(
			{ m_vectors[0].x - other.m_vectors[0].x,
			  m_vectors[0].y - other.m_vectors[0].y,
			  m_vectors[0].z - other.m_vectors[0].z,
			  m_vectors[0].w - other.m_vectors[0].w },

			{ m_vectors[1].x - other.m_vectors[1].x,
			  m_vectors[1].y - other.m_vectors[1].y,
			  m_vectors[1].z - other.m_vectors[1].z,
			  m_vectors[1].w - other.m_vectors[1].w },

			{ m_vectors[2].x - other.m_vectors[2].x,
			  m_vectors[2].y - other.m_vectors[2].y,
			  m_vectors[2].z - other.m_vectors[2].z,
			  m_vectors[2].w - other.m_vectors[2].w },

			{ m_vectors[3].x - other.m_vectors[3].x,
			  m_vectors[3].y - other.m_vectors[3].y,
			  m_vectors[3].z - other.m_vectors[3].z,
			  m_vectors[3].w - other.m_vectors[3].w });
	}

	void Mat4::operator+=(const Mat4& other) 
	{ 
		*this = *this + other; 
	}

	void Mat4::operator-=(const Mat4& other)
	{
		*this = *this - other;
	}

	const Vector4 Mat4::operator*(const Vector4& v) const
	{
		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		AE_PROFILE_FUNCTION();
		float x = (Get(0, 0) * v.x) + (Get(0, 1) * v.y) + (Get(0, 2) * v.z) + (Get(0, 3) * v.w);
		float y = (Get(1, 0) * v.x) + (Get(1, 1) * v.y) + (Get(1, 2) * v.z) + (Get(1, 3) * v.w);
		float z = (Get(2, 0) * v.x) + (Get(2, 1) * v.y) + (Get(2, 2) * v.z) + (Get(2, 3) * v.w);
		float w = (Get(3, 0) * v.x) + (Get(3, 1) * v.y) + (Get(3, 2) * v.z) + (Get(3, 3) * v.w);

		return Vector4(x, y, z, w);
	}
	
	Mat4 operator*(float k, const Mat4& m) { return m * k; }

	Vector4& Mat4::operator[](unsigned int index)
	{
		return m_vectors[index];
	}

	const Vector4& Mat4::operator[](unsigned int index) const
	{
		return m_vectors[index];
	}

	bool Mat4::operator==(const Mat4& other) const
	{
		return m_vectors[0] == other.m_vectors[0]
			&& m_vectors[1] == other.m_vectors[1]
			&& m_vectors[2] == other.m_vectors[2]
			&& m_vectors[3] == other.m_vectors[3];
	}

	bool Mat4::operator!=(const Mat4& other) const
	{
		return !(*this == other);
	}

	Mat4 Mat4::CalculateAdjugate() const
	{
		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		AE_PROFILE_FUNCTION();
		Mat3 m11 = Mat3(
			{ Get(1, 1), Get(2, 1), Get(3, 1) },
			{ Get(1, 2), Get(2, 2), Get(3, 2) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 m12 = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 2), Get(2, 2), Get(3, 2) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 m13 = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 1), Get(2, 1), Get(3, 1) },
			{ Get(1, 3), Get(2, 3), Get(3, 3) });

		Mat3 m14 = Mat3(
			{ Get(1, 0), Get(2, 0), Get(3, 0) },
			{ Get(1, 1), Get(2, 1), Get(3, 1) },
			{ Get(1, 2), Get(2, 2), Get(3, 2) });


		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		Mat3 m21 = Mat3(
			{ Get(0, 1), Get(2, 1), Get(3, 1) },
			{ Get(0, 2), Get(2, 2), Get(3, 2) },
			{ Get(0, 3), Get(2, 3), Get(3, 3) });

		Mat3 m22 = Mat3(
			{ Get(0, 0), Get(2, 0), Get(3, 0) },
			{ Get(0, 2), Get(2, 2), Get(3, 2) },
			{ Get(0, 3), Get(2, 3), Get(3, 3) });

		Mat3 m23 = Mat3(
			{ Get(0, 0), Get(2, 0), Get(3, 0) },
			{ Get(0, 1), Get(2, 1), Get(3, 1) },
			{ Get(0, 3), Get(2, 3), Get(3, 3) });

		Mat3 m24 = Mat3(
			{ Get(0, 0), Get(2, 0), Get(3, 0) },
			{ Get(0, 1), Get(2, 1), Get(3, 1) },
			{ Get(0, 2), Get(2, 2), Get(3, 2) });


		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		Mat3 m31 = Mat3(
			{ Get(0, 1), Get(1, 1), Get(3, 1) },
			{ Get(0, 2), Get(1, 2), Get(3, 2) },
			{ Get(0, 3), Get(1, 3), Get(3, 3) });

		Mat3 m32 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(3, 0) },
			{ Get(0, 2), Get(1, 2), Get(3, 2) },
			{ Get(0, 3), Get(1, 3), Get(3, 3) });

		Mat3 m33 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(3, 0) },
			{ Get(0, 1), Get(1, 1), Get(3, 1) },
			{ Get(0, 3), Get(1, 3), Get(3, 3) });

		Mat3 m34 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(3, 0) },
			{ Get(0, 1), Get(1, 1), Get(3, 1) },
			{ Get(0, 2), Get(1, 2), Get(3, 2) });


		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		Mat3 m41 = Mat3(
			{ Get(0, 1), Get(1, 1), Get(2, 1) },
			{ Get(0, 2), Get(1, 2), Get(2, 2) },
			{ Get(0, 3), Get(1, 3), Get(2, 3) });

		Mat3 m42 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(2, 0) },
			{ Get(0, 2), Get(1, 2), Get(2, 2) },
			{ Get(0, 3), Get(1, 3), Get(2, 3) });

		Mat3 m43 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(2, 0) },
			{ Get(0, 1), Get(1, 1), Get(2, 1) },
			{ Get(0, 3), Get(1, 3), Get(2, 3) });

		Mat3 m44 = Mat3(
			{ Get(0, 0), Get(1, 0), Get(2, 0) },
			{ Get(0, 1), Get(1, 1), Get(2, 1) },
			{ Get(0, 2), Get(1, 2), Get(2, 2) });

		//v1, v2, v3, v4

		//11, 12, 13, 14  x
		//21, 22, 23, 24  y
		//31, 32, 33, 34  z
		//41, 42, 43, 44  w

		Mat4 adjugate = Mat4(
			{ m11.Determinant(), -1.0f * m12.Determinant(), m13.Determinant(), -1.0f * m14.Determinant() },
			{ -1.0f * m21.Determinant(), m22.Determinant(), -1.0f * m23.Determinant(), m24.Determinant() },
			{ m31.Determinant(), -1.0f * m32.Determinant(), m33.Determinant(), -1.0f * m34.Determinant() },
			{ -1.0f * m41.Determinant(), m42.Determinant(), -1.0f * m43.Determinant(), m44.Determinant() });

		return adjugate;
	}


}