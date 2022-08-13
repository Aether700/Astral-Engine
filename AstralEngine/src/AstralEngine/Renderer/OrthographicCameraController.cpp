#include "aepch.h"
#include "OrthographicCameraController.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) :
		m_rotation(rotation), m_zoomLevel(1.0f), m_camRot(0.0f), m_camRotSpeed(180.0f), 
		m_camPos(Vector3::Zero()), m_camMoveSpeed(4.0f), m_zoomSpeed(0.25f), m_aspectRatio(aspectRatio), 
		m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel), 
		m_minZoom(0.25f), m_maxZoom(6.0f)
	{

	}

	OrthographicCameraController::~OrthographicCameraController() { }

	void OrthographicCameraController::OnUpdate()
	{
		AE_PROFILE_FUNCTION();
		if (Input::GetKey(KeyCode::A))
		{
			m_camPos.x -= m_camMoveSpeed * (float)Time::GetDeltaTime() * m_zoomLevel;
		}
		else if (Input::GetKey(KeyCode::D))
		{
			m_camPos.x += m_camMoveSpeed * (float)Time::GetDeltaTime() * m_zoomLevel;
		}

		if (Input::GetKey(KeyCode::W))
		{
			m_camPos.y += m_camMoveSpeed * (float)Time::GetDeltaTime() * m_zoomLevel;
		}
		else if (Input::GetKey(KeyCode::S))
		{
			m_camPos.y -= m_camMoveSpeed * (float)Time::GetDeltaTime() * m_zoomLevel;
		}
		m_camera.SetPosition(m_camPos);

		if (m_rotation)
		{
			if (Input::GetKey(KeyCode::Q))
			{
				m_camRot -= m_camRotSpeed * (float)Time::GetDeltaTime();
			}
			else if (Input::GetKey(KeyCode::E))
			{
				m_camRot += m_camRotSpeed * (float)Time::GetDeltaTime();
			}
			m_camera.SetRotation(m_camRot);
		}
	}

	void OrthographicCameraController::OnEvent(AEvent& e)
	{
		AEventDispatcher dispatcher(e);
		dispatcher.HandleAEvent<MouseScrolledEvent>(ADelegate<bool(MouseScrolledEvent&)>
			(function<&OrthographicCameraController::OnMouseScroll>, this));
		dispatcher.HandleAEvent<WindowResizeEvent>(ADelegate<bool(WindowResizeEvent&)>
			(function<&OrthographicCameraController::OnWindowResize>, this));
	}

	void OrthographicCameraController::SetZoomLevel(float zoomLevel)
	{
		m_zoomLevel = Math::Clamp(zoomLevel, m_minZoom, m_maxZoom);
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	bool OrthographicCameraController::OnMouseScroll(MouseScrolledEvent& e)
	{
		m_zoomLevel -= e.GetOffsetY() * m_zoomSpeed;
		m_zoomLevel = Math::Clamp(m_zoomLevel, m_minZoom, m_maxZoom);
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		m_aspectRatio = (float) e.GetWidth() / (float) e.GetHeight();
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

}