#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, 
			float nearClip = 0.001f, float farClip = 50.0f);
		~OrthographicCamera();

		void SetProjection(float left, float right, float bottom, float top, 
			float nearClip = 0.001f, float farClip = 1.0f);

		const Vector3& GetPosition() const;
		void SetPosition(const Vector3& pos);

		float GetRotation() const;
		void SetRotation(float rotation);

		const Mat4& GetProjectionMatrix() const;
		const Mat4& GetViewMatrix() const;
		const Mat4& GetViewProjectionMatrix() const;

	private:
		void RecalculateViewProjectionMatrix() const;

		Vector3 m_position;
		float m_rotationOnZ;
		
		float m_left;
		float m_right;
		float m_bottom;
		float m_top;
		float m_nearClip;
		float m_farClip;

		mutable bool m_isDirty;
		mutable Mat4 m_viewMatrix;
		mutable Mat4 m_projMatrix;
		mutable Mat4 m_viewProjMatrix;
	};
}