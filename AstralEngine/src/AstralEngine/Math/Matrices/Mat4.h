#pragma once
#include "AstralEngine/Math/Vectors/Vector4.h"
#include "AstralEngine/Math/Vectors/Vector3.h"

namespace AstralEngine
{
	class Mat4 //column order
	{
	public:
		Mat4();
		Mat4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4);
		Mat4(const Mat4& other);
		~Mat4();

		float Get(int x, int y) const;
		const float* Data() const;
		float Determinant() const;

		//returns the zero matrix if matrix is not invertable
		const Mat4 Inverse() const;
		const Mat4 Transpose() const;
		const float Trace() const;
		void Translate(const Vector3& v);
		void Rotate(float angle, const Vector3& axis);
		void Scale(const Vector3& scale);

		static const Mat4 Translate(const Mat4& m, const Vector3& v);
		static const Mat4 Rotate(const Mat4& m, float angle, const Vector3& axis);
		static const Mat4 Scale(const Mat4& m, const Vector3& scale);
		static const Mat4 Ortho(float left, float right, float bottom, float top);
		static const Mat4 Ortho(float left, float right, float bottom, float top, float nearVal, float farVal);
		static const Mat4 Perspective(float fov, float aspect, float nearVal, float farVal);
		static const Mat4 Identity();
		static const Mat4 Zero();

		const Mat4 operator*(float k) const;
		const Mat4 operator/(float k) const;
		const Mat4 operator*(const Mat4& other) const;
		const Mat4 operator+(const Mat4& other) const;
		const Mat4 operator-(const Mat4& other) const;
		void operator+=(const Mat4& other);
		void operator-=(const Mat4& other);

		const Vector4 operator*(const Vector4& v) const;
		Vector4& operator[](unsigned int index);
		const Vector4& operator[](unsigned int index) const;

		bool operator==(const Mat4& other) const;
		bool operator!=(const Mat4& other) const;

	private:
		Mat4 CalculateAdjugate() const;

		Vector4 m_vectors[4];
	};
	Mat4 operator*(float k, const Mat4& m);
}