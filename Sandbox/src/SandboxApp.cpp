#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
////////

//Scripts////////////////////////////////////////////////////////////////////////

class CamController : public AstralEngine::NativeScript
{
public:
	void OnStart() override
	{
		m_startPos = GetTransform().position;
	}
	
	void OnUpdate() override
	{
		auto& t = GetTransform();

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::W))
		{
			t.position -= t.Forward() * m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::S))
		{
			t.position += t.Forward() * m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::D))
		{
			t.position += t.Right() * m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::A))
		{
			t.position -= t.Right() * m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::Space))
		{
			t.position.y += m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftShift))
		{
			t.position.y -= m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::RightArrow))
		{
			auto euler = t.rotation.EulerAngles();
			euler.y -= m_rotSpeed * AstralEngine::Time::GetDeltaTime();
			t.rotation.SetEulerAngles(euler);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftArrow))
		{
			auto euler = t.rotation.EulerAngles();
			PrintEuler(euler);
			euler.y += m_rotSpeed * AstralEngine::Time::GetDeltaTime();
			PrintEuler(euler);
			t.rotation.SetEulerAngles(euler);
			PrintEuler(t.rotation.EulerAngles());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::R))
		{
			t.position = m_startPos;
			t.rotation = AstralEngine::Quaternion::Identity();
		}

	}

private:
	void PrintEuler(const AstralEngine::Vector3& euler)
	{
		AE_INFO("x: %f  y: %f  z: %f", euler.x, euler.y, euler.z);
	}

	float m_speed = 5.0f;
	float m_rotSpeed = 20.0f;

	AstralEngine::Vector3 m_startPos;
};

class Controller : public AstralEngine::NativeScript
{
public:
	void OnUpdate() override
	{
		AstralEngine::Transform& t = GetTransform();
		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::W))
		{
			t.position.y += speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::S))
		{
			t.position.y -= speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::D))
		{
			t.position.x += speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::A))
		{
			t.position.x -= speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKeyDown(AstralEngine::KeyCode::R))
		{
			t.position = AstralEngine::Vector3::Zero();
		}
	}

private:

	float speed = 0.05f;
};

class TargetMover : public AstralEngine::NativeScript
{
public:
	void OnUpdate() override
	{
		auto& t = GetTransform();
		t.position += m_velocity * AstralEngine::Time::GetDeltaTime();

		if (t.position.x > m_bounds.x)
		{
			t.position.x = m_bounds.x;
			m_velocity.x *= -1.0f;
		}
		
		if (t.position.x < -m_bounds.x)
		{
			t.position.x = -m_bounds.x;
			m_velocity.x *= -1.0f;
		}

		if (t.position.y > m_bounds.y)
		{
			t.position.y = m_bounds.y;
			m_velocity.y *= -1.0f;
		}

		if (t.position.y < -m_bounds.y)
		{
			t.position.y = -m_bounds.y;
			m_velocity.y *= -1.0f;
		}

		if (t.position.z > m_bounds.z)
		{
			t.position.z = m_bounds.z;
			m_velocity.z *= -1.0f;
		}

		if (t.position.z < -m_bounds.z)
		{
			t.position.z = -m_bounds.z;
			m_velocity.z *= -1.0f;
		}
	}

private:
	AstralEngine::Vector3 m_velocity = AstralEngine::Vector3(10.5f, 5.3f, 2.0f);
	AstralEngine::Vector3 m_bounds = AstralEngine::Vector3(3.0f, 3.0f, 3.0f);

};

class LookAtTester : public AstralEngine::NativeScript
{
public:
	void OnLateUpdate() override
	{
		if (m_target.IsValid())
		{
			auto& t = GetTransform();
			auto& target = m_target.GetTransform();
			t.LookAt(m_target.GetTransform());
			t.Forward();
		}
	}

	void SetTarget(AstralEngine::AEntity target) 
	{ 
		auto& pos = GetTransform().position;
		auto& targetPos = target.GetTransform().position;
		GetTransform().position = target.GetTransform().position + AstralEngine::Vector3(0, 0, 8);
		m_target = target; 
	}

private:
	AstralEngine::AEntity m_target;
};

void OnButtonClicked()
{
	static int count = 0;

	if (count == 5)
	{
		AE_INFO("next button click the application will close");
		count++;
	}
	else if (count == 6)
	{
		AstralEngine::Application::Exit();
	}
	else
	{
		AE_INFO("Application detected the button click %d times", ++count);
	}
}

//layer/////////////////////////////////

class TestLayer : public AstralEngine::Layer
{
public:

	void OnAttach() override
	{
		/*
		AstralEngine::AWindow* window = AstralEngine::Application::GetWindow();
		unsigned int width = window->GetWidth(), height = window->GetHeight();
		float aspectRatio = (float)width / (float)height;
		m_cameraController = AstralEngine::AReference<AstralEngine::OrthographicCameraController>::Create(aspectRatio, true);
		m_cameraController->SetZoomLevel(5.5f);

		AstralEngine::AReference<AstralEngine::UIWindow> uiWindow 
			= AstralEngine::UIContext::CreateUIWindow({ 300, 300 }, 200, 200);
		AstralEngine::UIContext::CreateUIWindow({ 800, 300 }, 200, 200);

		AstralEngine::AReference<AstralEngine::UIButton> button 
			= AstralEngine::AReference<AstralEngine::UIButton>::Create("My Button", AstralEngine::Vector4(0.8f, 0, 0, 1));
		uiWindow->AddElement((AstralEngine::AReference<AstralEngine::UIElement>)button);
		button->SetParent(uiWindow);
		button->AddListener(AstralEngine::ADelegate<void()>(&OnButtonClicked));
		m_texture = AstralEngine::Texture2D::Create("assets/textures/septicHanzo.png");
		*/

		//m_framebuffer = AstralEngine::Framebuffer::Create(width, height);

		m_scene = AstralEngine::AReference<AstralEngine::Scene>::Create();
		m_entity = m_scene->CreateAEntity();
		
		m_entity.EmplaceComponent<AstralEngine::SpriteRenderer>(0, 1, 0, 1);
		//m_entity.EmplaceComponent<Controller>();
		
		auto e = m_scene->CreateAEntity();
		e.EmplaceComponent<TargetMover>();
		e.EmplaceComponent<AstralEngine::SpriteRenderer>(1, 0, 1, 1);

		m_entity.EmplaceComponent<LookAtTester>().SetTarget(e);

		AstralEngine::AEntity cam = AstralEngine::AEntity((AstralEngine::BaseEntity)0, m_scene.Get());
		//cam.GetComponent<AstralEngine::Camera>().camera.SetProjectionType(AstralEngine::SceneCamera::ProjectionType::Perspective);
		m_scene->DestroyAEntity(cam);
		cam = m_scene->CreateAEntity();
		cam.EmplaceComponent<AstralEngine::Camera>().camera.SetProjectionType(AstralEngine::SceneCamera::ProjectionType::Perspective);
		cam.GetTransform().position.z = 8.0f;
		cam.EmplaceComponent<CamController>();
		//cam.EmplaceComponent<LookAtTester>().SetTarget(m_entity);
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();
		auto* window = AstralEngine::Application::GetWindow();
		m_scene->OnViewportResize(window->GetWidth(), window->GetHeight());

		/*
		m_cameraController->OnUpdate();


		AstralEngine::RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		AstralEngine::RenderCommand::Clear();
		AstralEngine::Renderer::BeginScene(m_cameraController->GetCamera());

		AstralEngine::Application::GetUIContext()->TempUpdate();

		/*
		AstralEngine::Renderer2D::DrawQuad(AstralEngine::Mat4::Identity(), { 1, 1, 1, 1 });
		AstralEngine::Renderer::DrawQuad(AstralEngine::Mat4::Identity());
		//AstralEngine::Renderer2D::DrawUIWindow(m_uiWindow);


		AstralEngine::Renderer::EndScene();
		*/

		AstralEngine::Renderer::ResetStats();
	}

	/*
	bool OnEvent(AstralEngine::AEvent& e) override
	{
		m_cameraController->OnEvent(e);
		return false;
	}
	*/

private:
	AstralEngine::AReference<AstralEngine::OrthographicCameraController> m_cameraController;

	//AstralEngine::UIWindow m_uiWindow = AstralEngine::UIWindow({ 300, 300 }, 200, 200);//, {1, 0, 0, 1});

	bool m_wasHovered = false;

	float m_timer = 0.2f;
	float m_currTimer = 0.0f;

	
	AstralEngine::AReference<AstralEngine::Texture2D> m_texture;
	AstralEngine::AReference<AstralEngine::Framebuffer> m_framebuffer;
	AstralEngine::AReference<AstralEngine::Scene> m_scene;
	AstralEngine::AEntity m_entity;
};


class Sandbox : public AstralEngine::Application
{
public:
	Sandbox()
	{
		AttachOverlay(new TestLayer());
	}
};

AstralEngine::Application* CreateApp()
{
	return new Sandbox();
}