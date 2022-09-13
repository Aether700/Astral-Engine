#include "aepch.h"
#include "AstralEngine/Core/Time.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/Renderer/RenderCommand.h"
#include "AstralEngine/Core/Application.h"
#include "Scene.h"
#include "AEntity.h"
#include "Components.h"

namespace AstralEngine
{
	class EditorCameraController : public NativeScript
	{
	public:

		EditorCameraController() { }

		void OnStart() override
		{
			m_zoomLevel = 1.0f;
			m_camRotSpeed = Math::DegreeToRadians(180.0f);
			m_camMoveSpeed = 4.5f;
			m_zoomSpeed = 0.25f; 
			m_minZoom = 0.25f; 
			m_maxZoom = 6.0f;
		}

		void EnableRotation(bool val)
		{
			m_rotation = val;
		}

		void OnUpdate() override
		{
			AE_PROFILE_FUNCTION();
			Transform& t = GetTransform();

			if (Input::GetKey(KeyCode::A))
			{
				Vector3 right = t.Right();
				t.SetLocalPosition(t.GetLocalPosition() - right * m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel);
			}
			
			if (Input::GetKey(KeyCode::D))
			{
				Vector3 right = t.Right();
				t.SetLocalPosition(t.GetLocalPosition() + right * m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel);
			}

			if (Input::GetKey(KeyCode::W))
			{
				Vector3 forward = t.Forward();
				t.SetLocalPosition(t.GetLocalPosition() + forward * m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel);
			}
			
			if (Input::GetKey(KeyCode::S))
			{
				Vector3 forward = t.Forward();
				t.SetLocalPosition(t.GetLocalPosition() - forward * m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel);
			}
			
			if (m_rotation)
			{
				if (Input::GetKey(KeyCode::LeftArrow))
				{
					Quaternion newRotation = Quaternion::AngleAxisToQuaternion(m_camRotSpeed * 
						Time::GetDeltaTime(), t.Up()) * t.GetRotation();
					t.SetRotation(newRotation);
				}

				if (Input::GetKey(KeyCode::RightArrow))
				{
					Quaternion newRotation = Quaternion::AngleAxisToQuaternion(-m_camRotSpeed *
						Time::GetDeltaTime(), t.Up()) * t.GetRotation();
					t.SetRotation(newRotation);
				}

				if (Input::GetKey(KeyCode::UpArrow))
				{
					Quaternion newRotation = Quaternion::AngleAxisToQuaternion(m_camRotSpeed *
						Time::GetDeltaTime(), t.Right()) * t.GetRotation();
					t.SetRotation(newRotation);
				}

				if (Input::GetKey(KeyCode::DownArrow))
				{
					Quaternion newRotation = Quaternion::AngleAxisToQuaternion(-m_camRotSpeed *
						Time::GetDeltaTime(), t.Right()) * t.GetRotation();
					t.SetRotation(newRotation);
				}
			}
		}

	private:
		float m_camMoveSpeed;
		float m_camRotSpeed;
		float m_zoomLevel;
		float m_minZoom;
		float m_maxZoom;
		float m_zoomSpeed;
		bool m_rotation;
	};


	Scene::Scene(bool rotation)
	{
		AEntity camera = CreateAEntity();
		camera.GetTransform().SetLocalPosition(0.0f, 0.0f, -1.0f);
		camera.EmplaceComponent<Camera>().SetAsMain(true);
		EditorCameraController& controller = camera.EmplaceComponent<EditorCameraController>();
		controller.EnableRotation(rotation);

		AWindow* window = Application::GetWindow();
		OnViewportResize(window->GetWidth(), window->GetHeight());
	}

	AEntity Scene::CreateAEntity()
	{
		AEntity e = AEntity(m_registry.CreateEntity(), this);
		e.EmplaceComponent<Transform>();
		e.EmplaceComponent<AEntityData>();
		return e;
	}

	//destroys an entity at the end of the frame
	void Scene::DestroyAEntity(AEntity e)
	{
		m_entitiesToDestroy.Add(e);
	}

	void Scene::OnUpdate()
	{
		//temp//////////////////////////////////////
		//call start on scripts
		{
			static bool start = false;

			if (!start)
			{
				CallOnStart();
				start = true;
			}
		}
		////////////////////////////////////////////

		Camera* mainCamera = nullptr;
		Transform* cameraTransform;
		
		AEntity mainCameraEntity = Camera::GetMainCamera();
		if (mainCameraEntity.IsValid())
		{
			mainCamera = &mainCameraEntity.GetComponent<Camera>();
			cameraTransform = &mainCameraEntity.GetTransform();
			const Vector3& backgroundColor = mainCamera->GetBackgroundColor();
			RenderCommand::SetClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
		}
		else
		{
			RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
		RenderCommand::Clear();

		if (mainCamera != nullptr)
		{
			AE_PROFILE_SCOPE("Rendering");

			Renderer::BeginScene(*mainCamera, *cameraTransform);
			
			auto group = m_registry.GetGroup<AEntityData, Transform, RenderData>();

			for (BaseEntity e : group)
			{
				auto [data, transform, render] = group.Get<AEntityData, Transform, RenderData>(e);
				if (data.IsActive())
				{
					if (render.IsActive())
					{
						render.SendToRenderer(transform);
					}
				}
			}

			Renderer::EndScene();
		}

		//update Scripts
		CallOnUpdate();
		CallOnLateUpdate();

		//destroy all entities enqueued for destruction this frame
		DestroyEntitiesToDestroy();
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto& view = m_registry.GetView<Camera>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.Get<Camera>(entity);
			if (!cameraComponent.IsFixedAspectRatio())
			{
				cameraComponent.GetCamera().SetViewportSize(width, height);
			}
		}
	}

	void Scene::CallOnStart()
	{
		auto view = m_registry.GetView<CallbackList>();
		for (BaseEntity e : view)
		{
			auto& list = view.Get<CallbackList>(e);
			list.CallOnStart();
		}
	}

	void Scene::CallOnUpdate()
	{
		auto view = m_registry.GetView<CallbackList>();
		
		for (BaseEntity e : view)
		{
			auto& list = view.Get<CallbackList>(e);
			list.CallOnUpdate();
		}
	}

	void Scene::CallOnLateUpdate()
	{
		auto view = m_registry.GetView<CallbackList>();
		for (BaseEntity e : view)
		{
			auto& list = view.Get<CallbackList>(e);
			list.CallOnLateUpdate();
		}
	}

	void Scene::DestroyEntitiesToDestroy()
	{
		for (AEntity& e : m_entitiesToDestroy)
		{
			m_registry.DeleteEntity(e);
		}
		m_entitiesToDestroy.Clear();
	}
}