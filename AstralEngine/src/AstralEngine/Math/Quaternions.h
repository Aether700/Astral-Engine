#pragma once
#include "Vectors/Vector3.h"
#include "Matrices/Mat4.h"

namespace AstralEngine
{
	// Internally stores the data as radians but the API is in degrees
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float w, const Vector3& v);
		Quaternion(float w, float x, float y, float z);

		float GetW() const;
		float GetX() const;
		float GetY() const;
		float GetZ() const;
		
		void SetW(float w);
		void SetX(float x);
		void SetY(float y);
		void SetZ(float z);
		void Set(float w, float x, float y, float z);

		Quaternion Conjugate() const;
		float Magnitude() const;
		float SqrMagnitude() const;

		void Normalize();
		Quaternion Inverse() const;

		Mat3 ComputeRotationMatrix() const;

		// all euler angles are in degrees
		Vector3 EulerAngles() const;
		void SetEulerAngles(const Vector3& euler);
		void SetEulerAngles(float x, float y, float z);

		static Quaternion Identity();

		// takes euler angles as degrees and outputs a unit Quaternion object
		static Quaternion EulerToQuaternion(const Vector3& euler);
		static Quaternion EulerToQuaternion(float x, float y, float z);

		// returns a normalized quaternion which has the specified angle of rotation around the provided axis. 
		// The angle is in degrees
		static Quaternion AngleAxisToQuaternion(float angle, const Vector3& axis);

		static Quaternion Normalize(const Quaternion& q);
		static float DotProduct(const Quaternion& q1, const Quaternion& q2);

		// returns an angle in degrees
		static float Angle(const Quaternion& q1, const Quaternion& q2);
		static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
		static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

		// returns the identity if either vector is the vector 0
		static Quaternion LookRotation(const Vector3& lookDir, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));
		static Quaternion FromToRotation(const Vector3& from, const Vector3& to);

		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator*(float k) const;
		Vector3 operator*(const Vector3& v) const;
		Quaternion operator/(float k) const;

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;

	private:
		// rotations are stores as radians not degrees
		float m_w;
		Vector3 m_v; // can be visualized as axis of rotation while w is the angle of rotation
	};

	Quaternion operator*(float k, const Quaternion& q);
}
