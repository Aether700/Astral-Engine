#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	//runtime camera (position and rotation handled by transform component)
	class RuntimeCamera 
	{
	public:
		RuntimeCamera() : m_projectionMatrix(Mat4::Identity()) { }
		RuntimeCamera(const Mat4& projection) : m_projectionMatrix(projection) { }

		const Mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

		bool operator==(const RuntimeCamera& other) const
		{
			return m_projectionMatrix == other.m_projectionMatrix;
		}

		bool operator!=(const RuntimeCamera& other) const
		{
			return !(*this == other);
		}

	protected:
		Mat4 m_projectionMatrix;
	};

	class SceneCamera : public RuntimeCamera
	{
	public:

		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1
		};

		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);

		void SetViewportSize(unsigned int width, unsigned int height);

		float GetPerspectiveVerticalFOV() const { return m_perspectiveFOV; }
		void SetPerspectiveVerticalFOV(float fov) { m_perspectiveFOV = fov; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_perspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_perspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; RecalculateProjection(); }


		float GetOrthographicSize() const { return m_orthographicSize; }
		void SetOrthographicSize(float size) { m_orthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_orthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_orthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_orthographicFar = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_projectionType; }
		void SetProjectionType(ProjectionType type) { m_projectionType = type; RecalculateProjection(); }

		bool operator==(const SceneCamera& other) const;
		bool operator!=(const SceneCamera& other) const;

	private:
		void RecalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic; //should change default when implement 3D

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		float m_perspectiveFOV = Math::DegreeToRadiants(45.0f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

		float m_aspectRatio = 0.0f;
	};
}