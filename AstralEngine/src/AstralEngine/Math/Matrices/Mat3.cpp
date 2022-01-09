#include "aepch.h"
#include "Mat3.h"
#include "AstralEngine/Math/Utils.h"

namespace AstralEngine
{
	Mat3::Mat3()
	{
		m_vectors[0] = Vector3::Zero();
		m_vectors[1] = Vector3::Zero();
		m_vectors[2] = Vector3::Zero();
	}

	Mat3::Mat3(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		m_vectors[0] = v1;
		m_vectors[1] = v2;
		m_vectors[2] = v3;
	}

	Mat3::Mat3(const Mat3& other)
	{
		m_vectors[0] = other[0];
		m_vectors[1] = other[1];
		m_vectors[2] = other[2];
	}
	
	Mat3::~Mat3() { }

	float Mat3::Get(int x, int y) const
	{
		return m_vectors[y][x];
	}

	const float* Mat3::Data() const
	{
		return m_vectors[0].Data();
	}

	float Mat3::Determinant() const
	{
		// 11, 12, 13
		// 21, 22, 23
		// 31, 32, 33

		AE_PROFILE_FUNCTION();
		float determinant;

		determinant =  Get(0, 0) * ((Get(1, 1) * Get(2, 2)) - (Get(2, 1) * Get(1, 2)));
		determinant -= Get(0, 1) * ((Get(1, 0) * Get(2, 2)) - (Get(2, 0) * Get(1, 2)));
		determinant += Get(0, 2) * ((Get(1, 0) * Get(2, 1)) - (Get(2, 0) * Get(1, 1)));
		return determinant;
	}

	//not done
	const Mat3 Mat3::Inverse() const
	{
		AE_PROFILE_FUNCTION();
		float det = Determinant();

		if (det == 0.0f)
		{
			return Mat3::Zero();
		}

		Mat3 adjugate = CalculateAdjugate();
		return adjugate / det;
	}

	const Mat3 Mat3::Transpose() const
	{
		// v1, v2, v3

		// 11, 12, 13  x
		// 21, 22, 23  y
		// 31, 32, 33  z

		return Mat3(
			{ Get(0, 0), Get(0, 1) , Get(0, 2) },
			{ Get(1, 0), Get(1, 1) , Get(1, 2) },
			{ Get(2, 0), Get(2, 1) , Get(2, 2) });
	}

	const float Mat3::Trace() const
	{
		return Get(0, 0) + Get(1, 1) + Get(2, 2);
	}

	const Mat3 Mat3::Identity() { return Mat3({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }); }

	const Mat3 Mat3::Zero() { return Mat3(); }

	const Mat3 Mat3::operator*(float k) const
	{
		return Mat3(
			{ m_vectors[0].x * k,  m_vectors[0].y * k, m_vectors[0].z * k },
			{ m_vectors[1].x * k,  m_vectors[1].y * k, m_vectors[1].z * k },
			{ m_vectors[2].x * k,  m_vectors[2].y * k, m_vectors[2].z * k });
	}

	const Mat3 Mat3::operator/(float k) const
	{
		float inverseScalar = 1.0f / k;
		return *this * inverseScalar;
	}

	const Mat3 Mat3::operator*(const Mat3& other) const
	{
		// v1, v2, v3

		// 11, 12, 13  x
		// 21, 22, 23  y
		// 31, 32, 33  z

		AE_PROFILE_FUNCTION();
		float f11 = (Get(0, 0) * other.Get(0, 0)) + (Get(0, 1) * other.Get(1, 0)) + (Get(0, 2) * other.Get(2, 0));
		float f12 = (Get(0, 0) * other.Get(0, 1)) + (Get(0, 1) * other.Get(1, 1)) + (Get(0, 2) * other.Get(2, 1));
		float f13 = (Get(0, 0) * other.Get(0, 2)) + (Get(0, 1) * other.Get(1, 2)) + (Get(0, 2) * other.Get(2, 2));

		float f21 = (Get(1, 0) * other.Get(0, 0)) + (Get(1, 1) * other.Get(1, 0)) + (Get(1, 2) * other.Get(2, 0));
		float f22 = (Get(1, 0) * other.Get(0, 1)) + (Get(1, 1) * other.Get(1, 1)) + (Get(1, 2) * other.Get(2, 1));
		float f23 = (Get(1, 0) * other.Get(0, 2)) + (Get(1, 1) * other.Get(1, 2)) + (Get(1, 2) * other.Get(2, 2));

		float f31 = (Get(2, 0) * other.Get(0, 0)) + (Get(2, 1) * other.Get(1, 0)) + (Get(2, 2) * other.Get(2, 0));
		float f32 = (Get(2, 0) * other.Get(0, 1)) + (Get(2, 1) * other.Get(1, 1)) + (Get(2, 2) * other.Get(2, 1));
		float f33 = (Get(2, 0) * other.Get(0, 2)) + (Get(2, 1) * other.Get(1, 2)) + (Get(2, 2) * other.Get(2, 2));

		return Mat3(
			{f11, f21, f31}, 
			{f12, f22, f32}, 
			{f13, f23, f33});
	}


	const Mat3 Mat3::operator+(const Mat3& other) const
	{
		return Mat3(
			{ m_vectors[0].x + other.m_vectors[0].x, 
			  m_vectors[0].y + other.m_vectors[0].y,
			  m_vectors[0].z + other.m_vectors[0].z },

			{ m_vectors[1].x + other.m_vectors[1].x,  
			  m_vectors[1].y + other.m_vectors[1].y, 
			  m_vectors[1].z + other.m_vectors[1].z },

			{ m_vectors[2].x + other.m_vectors[2].x,
			  m_vectors[2].y + other.m_vectors[2].y,
			  m_vectors[2].z + other.m_vectors[2].z });
	}

	const Mat3 Mat3::operator-(const Mat3& other) const
	{
		return Mat3(
			{ m_vectors[0].x - other.m_vectors[0].x,
			  m_vectors[0].y - other.m_vectors[0].y,
			  m_vectors[0].z - other.m_vectors[0].z },

			{ m_vectors[1].x - other.m_vectors[1].x,
			  m_vectors[1].y - other.m_vectors[1].y,
			  m_vectors[1].z - other.m_vectors[1].z },

			{ m_vectors[2].x - other.m_vectors[2].x,
			  m_vectors[2].y - other.m_vectors[2].y,
			  m_vectors[2].z - other.m_vectors[2].z });
	}

	void Mat3::operator+=(const Mat3& other) { *this = *this + other; }
	void Mat3::operator-=(const Mat3& other) { *this = *this - other; }

	const Vector3 Mat3::operator*(const Vector3& v) const
	{
		// v1, v2, v3

		// 11, 12, 13  x
		// 21, 22, 23  y
		// 31, 32, 33  z

		AE_PROFILE_FUNCTION();
		float x = (Get(0, 0) * v.x) + (Get(0, 1) * v.y) + (Get(0, 2) * v.z);
		float y = (Get(1, 0) * v.x) + (Get(1, 1) * v.y) + (Get(1, 2) * v.z);
		float z = (Get(2, 0) * v.x) + (Get(2, 1) * v.y) + (Get(2, 2) * v.z);

		return Vector3(x, y, z);
	}


	Mat3 operator*(float k, const Mat3& m) { return m * k; }

	const Vector3& Mat3::operator[](unsigned int index) const
	{
		return m_vectors[index];
	}

	bool Mat3::operator==(const Mat3& other) const
	{
		return m_vectors[0] == other.m_vectors[0]
			&& m_vectors[1] == other.m_vectors[1]
			&& m_vectors[2] == other.m_vectors[2];
	}

	bool Mat3::operator!=(const Mat3& other) const
	{
		return !(*this == other);
	}

	Mat3 Mat3::CalculateAdjugate() const
	{
		// 11, 12, 13
		// 21, 22, 23
		// 31, 32, 33

		AE_PROFILE_FUNCTION();
		float det11 = ((Get(1, 1) * Get(2, 2)) -
			(Get(2, 1) * Get(1, 2)));
		float det12 = ((Get(1, 0) * Get(2, 2)) -
			(Get(2, 0) * Get(1, 2)));
		float det13 = ((Get(1, 0) * Get(2, 1)) -
			(Get(2, 0) * Get(1, 1)));

		float det21 = ((Get(0, 1) * Get(2, 2)) -
			(Get(2, 1) * Get(0, 2)));
		float det22 = ((Get(0, 0) * Get(2, 2)) -
			(Get(2, 0) * Get(0, 2)));
		float det23 = ((Get(0, 0) * Get(2, 1)) -
			(Get(2, 0) * Get(0, 1)));


		float det31 = ((Get(0, 1) * Get(1, 2)) -
			(Get(1, 1) * Get(0, 2)));
		float det32 = ((Get(0, 0) * Get(1, 2)) -
			(Get(1, 0) * Get(0, 2)));
		float det33 = ((Get(0, 0) * Get(1, 1)) -
			(Get(1, 0) * Get(0, 1)));

		// 11, 12, 13
		// 21, 22, 23
		// 31, 32, 33

		return Mat3(
			{ det11, -1.0f * det12, det13 },
			{ -1.0f * det21, det22, -1.0f * det23 },
			{ det31, -1.0f * det32, det33 });
	}

}