#include "aepch.h"
#include "OrthographicCamera.h"

namespace AstralEngine
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearClip, float farClip) 
		: m_left(left), m_right(right), m_bottom(bottom), m_top(top), m_nearClip(nearClip), 
		m_farClip(farClip), m_position(Vector3::Zero()),
		m_rotationOnZ(0.0f), m_isDirty(true) { }

	OrthographicCamera::~OrthographicCamera() { }

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, 
		float nearClip, float farClip)
	{
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_nearClip = nearClip;
		m_farClip = farClip;

		m_isDirty = true;
	}
	
	const Vector3& OrthographicCamera::GetPosition() const
	{
		return m_position; 
	}
	
	void OrthographicCamera::SetPosition(const Vector3& pos) 
	{
		m_position = pos; 
		m_isDirty = true;
	}

	float OrthographicCamera::GetRotation() const 
	{
		return m_rotationOnZ; 
	}

	void OrthographicCamera::SetRotation(float rotation)
	{
		m_rotationOnZ = rotation;
		m_isDirty = true;
	}

	const Mat4& OrthographicCamera::GetProjectionMatrix() const 
	{
		if (m_isDirty) 
		{
			RecalculateViewProjectionMatrix();
		}
		return m_projMatrix; 
	}
	
	const Mat4& OrthographicCamera::GetViewMatrix() const
	{
		if (m_isDirty)
		{
			RecalculateViewProjectionMatrix();
		}
		return m_viewMatrix; 
	}
	
	const Mat4& OrthographicCamera::GetViewProjectionMatrix() const 
	{
		if (m_isDirty)
		{
			RecalculateViewProjectionMatrix();
		}
		return m_viewProjMatrix; 
	}

	void OrthographicCamera::RecalculateViewProjectionMatrix() const
	{
		AE_PROFILE_FUNCTION();
		Mat4 transform = Mat4::Translate(Mat4::Identity(), m_position) * 
			Mat4::Rotate(Mat4::Identity(), Math::DegreeToRadians(m_rotationOnZ), Vector3(0, 0, 1));
		m_projMatrix = Mat4::Ortho(m_left, m_right, m_bottom, m_top, m_nearClip, m_farClip);
		m_viewMatrix = transform.Inverse();
		m_viewProjMatrix = m_projMatrix * m_viewMatrix;
		m_isDirty = false;
	}
}