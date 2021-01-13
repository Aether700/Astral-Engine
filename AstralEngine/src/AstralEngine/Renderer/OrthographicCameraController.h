#pragma once
#include "OrthographicCamera.h"
#include "AstralEngine/AEvents/AppEvents.h"

namespace AstralEngine
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);
		~OrthographicCameraController();

		inline OrthographicCamera& GetCamera() { return m_camera; }
		inline const OrthographicCamera& GetCamera() const { return m_camera; }

		void OnUpdate();
		void OnEvent(AEvent& e);
		
		void SetZoomLevel(float zoomLevel);


	private:
		bool OnMouseScroll(MouseScrolledEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);


		Vector3 m_camPos;
		float m_camMoveSpeed;
		float m_camRotSpeed;
		float m_camRot;
		float m_zoomLevel;
		float m_minZoom;
		float m_maxZoom;
		float m_zoomSpeed;
		float m_aspectRatio;
		bool m_rotation;
		OrthographicCamera m_camera;
	};
}