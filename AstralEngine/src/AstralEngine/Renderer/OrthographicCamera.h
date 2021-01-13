#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		~OrthographicCamera();

		void SetProjection(float left, float right, float bottom, float top);

		const Vector3& GetPosition() const;
		void SetPosition(const Vector3& pos);

		float GetRotation() const;
		void SetRotation(float rotation);

		const Mat4& GetProjectionMatrix() const;
		const Mat4& GetViewMatrix() const;
		const Mat4& GetViewProjectionMatrix() const;

	private:
		void RecalculateViewProjectionMatrix();

		Vector3 m_position;
		float m_rotationOnZ;
		Mat4 m_projMatrix;
		Mat4 m_viewMatrix;
		Mat4 m_viewProjMatrix;
	};
}