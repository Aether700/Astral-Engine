#include "aepch.h"
#include "SceneCamera.h"

namespace AstralEngine
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Orthographic;

		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Perspective;

		m_perspectiveFOV = fov;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(unsigned int width, unsigned int height)
	{
		m_aspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	bool SceneCamera::operator==(const SceneCamera& other) const
	{
		if(m_projectionType != other.m_projectionType)
		{
			return false;
		}

		bool common = RuntimeCamera::operator==(other) && m_aspectRatio == other.m_aspectRatio;
		bool typeSpecific;

		if (m_projectionType == ProjectionType::Perspective)
		{
			typeSpecific = m_perspectiveFOV == other.m_perspectiveFOV
						&& m_perspectiveFar == other.m_perspectiveFar
						&& m_perspectiveNear == other.m_perspectiveNear;
		}
		else
		{
			typeSpecific = m_orthographicSize == other.m_orthographicSize
				        && m_orthographicFar  == other.m_orthographicFar
				        && m_orthographicNear == other.m_orthographicNear;
		}

		return common && typeSpecific;
	}

	bool SceneCamera::operator!=(const SceneCamera& other) const
	{
		return !(*this == other);
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_projectionType == ProjectionType::Perspective)
		{
			m_projectionMatrix = Mat4::Perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
		}
		else //Orthographic
		{
			float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoBottom = -m_orthographicSize * 0.5f;
			float orthoTop = m_orthographicSize * 0.5f;

			m_projectionMatrix = Mat4::Ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
		}
	}
}