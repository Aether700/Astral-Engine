#include "aepch.h"
#include "AstralEngine/Core/Time.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/Renderer/RenderCommand.h"
#include "Core/Application.h"
#include "Scene.h"
#include "AEntity.h"
#include "Components.h"

namespace AstralEngine
{
	class EditorCameraController : public NativeScript
	{
	public:

		EditorCameraController() { AE_CORE_INFO("Editor Camera Controller constructor called"); }

		void OnStart() override
		{
			m_zoomLevel = 1.0f;
			m_camRotSpeed = Math::DegreeToRadiants(180.0f);
			m_camMoveSpeed = 4.5f;
			m_zoomSpeed = 0.25f; 
			m_minZoom = 0.25f; 
			m_maxZoom = 6.0f;
		}

		void EnableRotation(bool val)
		{
			m_rotation = val;
		}

		//moving camera is independant of rotation of the camera (might want to fix later)
		void OnUpdate() override
		{
			AE_PROFILE_FUNCTION();
			if (Input::GetKey(KeyCode::A))
			{
				GetTransform().position.x -= m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel;
			}
			else if (Input::GetKey(KeyCode::D))
			{
				GetTransform().position.x += m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel;
			}

			if (Input::GetKey(KeyCode::W))
			{
				GetTransform().position.y += m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel;
			}
			else if (Input::GetKey(KeyCode::S))
			{
				GetTransform().position.y -= m_camMoveSpeed * Time::GetDeltaTime() * m_zoomLevel;
			}
			
			if (m_rotation)
			{
				if (Input::GetKey(KeyCode::Q))
				{
					GetTransform().rotation.z += m_camRotSpeed * Time::GetDeltaTime();
				}
				else if (Input::GetKey(KeyCode::E))
				{
					GetTransform().rotation.z -= m_camRotSpeed * Time::GetDeltaTime();
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
		camera.GetTransform().position.z = -1.0f;
		camera.EmplaceComponent<Camera>();
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

		RuntimeCamera* mainCamera = nullptr;
		Transform* cameraTransform;
		auto camView = m_registry.GetView<Camera, Transform>();
		for (auto entity : camView)
		{
			auto[transform, camera] = camView.Get<Transform, Camera>(entity);

			if (camera.primary)
			{
				mainCamera = &camera.camera;
				cameraTransform = &transform;
				break;
			}
		}

		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();

		if (mainCamera != nullptr)
		{
			Renderer::BeginScene(*mainCamera, *cameraTransform);
			auto group = m_registry.GetGroup<Transform, SpriteRenderer, AEntityData>();

			for (BaseEntity e : group)
			{
				auto& components = group.Get<Transform, SpriteRenderer, AEntityData>(e);
				AEntityData& data = std::get<AEntityData&>(components);

				if (data.IsActive())
				{
					SpriteRenderer& sprite = std::get<SpriteRenderer&>(components);
					if (sprite.IsActive())
					{
						Transform& transform = std::get<Transform&>(components);
						Renderer::DrawSprite(transform.GetTransformMatrix(), sprite);
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
			if (!cameraComponent.fixedAspectRatio)
			{
				cameraComponent.camera.SetViewportSize(width, height);
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