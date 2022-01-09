#pragma once
#include "AstralEngine/Math/Vectors/Vector3.h"

namespace AstralEngine
{
	class Mat3 //column major
	{
	public:
		Mat3();
		Mat3(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		Mat3(const Mat3& other);
		~Mat3();

		float Get(int x, int y) const;
		const float* Data() const;
		float Determinant() const;
		
		//returns the zero matrix if matrix is not invertable
		const Mat3 Inverse() const;
		const Mat3 Transpose() const;
		const float Trace() const;

		static const Mat3 Identity();
		static const Mat3 Zero();

		const Mat3 operator*(float k) const;
		const Mat3 operator/(float k) const;
		const Mat3 operator*(const Mat3& other) const;
		const Mat3 operator+(const Mat3& other) const;
		const Mat3 operator-(const Mat3& other) const;
		void operator+=(const Mat3& other);
		void operator-=(const Mat3& other);
		
		const Vector3 operator*(const Vector3& v) const;
		const Vector3& operator[](unsigned int index) const;

		bool operator==(const Mat3& other) const;
		bool operator!=(const Mat3& other) const;

	private:
		Mat3 CalculateAdjugate() const;

		Vector3 m_vectors[3];
	};

	Mat3 operator*(float k, const Mat3& m);
}