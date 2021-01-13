#include "aepch.h"
#include "OrthographicCamera.h"

namespace AstralEngine
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) 
		: m_projMatrix(Mat4::Ortho(left, right, bottom, top)), m_viewMatrix(Mat4::Identity()), m_position(Vector3::Zero()), 
		m_rotationOnZ(0.0f)
	{
		m_viewProjMatrix = m_projMatrix * m_viewMatrix;
	}

	OrthographicCamera::~OrthographicCamera() { }

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_projMatrix = Mat4::Ortho(left, right, bottom, top);
		m_viewProjMatrix = m_projMatrix * m_viewMatrix;
	}

	const Vector3& OrthographicCamera::GetPosition() const 
	{
		return m_position; 
	}
	
	void OrthographicCamera::SetPosition(const Vector3& pos) 
	{
		m_position = pos; 
		RecalculateViewProjectionMatrix();
	}

	float OrthographicCamera::GetRotation() const 
	{
		return m_rotationOnZ; 
	}

	void OrthographicCamera::SetRotation(float rotation)
	{
		m_rotationOnZ = rotation;
		RecalculateViewProjectionMatrix();
	}

	const Mat4& OrthographicCamera::GetProjectionMatrix() const 
	{
		return m_projMatrix; 
	}
	
	const Mat4& OrthographicCamera::GetViewMatrix() const
	{
		return m_viewMatrix; 
	}
	
	const Mat4& OrthographicCamera::GetViewProjectionMatrix() const 
	{
		return m_viewProjMatrix; 
	}

	void OrthographicCamera::RecalculateViewProjectionMatrix()
	{
		AE_PROFILE_FUNCTION();
		Mat4 transform = Mat4::Translate(Mat4::Identity(), m_position) * 
			Mat4::Rotate(Mat4::Identity(), Math::DegreeToRadiants(m_rotationOnZ), Vector3(0, 0, 1));

		m_viewMatrix = transform.Inverse();
		m_viewProjMatrix = m_projMatrix * m_viewMatrix;
	}
}