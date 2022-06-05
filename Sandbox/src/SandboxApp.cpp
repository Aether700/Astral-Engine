#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
////////

//Scripts////////////////////////////////////////////////////////////////////////

class QuaternionTester : public AstralEngine::NativeScript
{
public:
	void OnCreate()
	{
		m_q = AstralEngine::Quaternion::EulerToQuaternion(50.0f, 10.0f, 5.0f);

		AstralEngine::Quaternion q = AstralEngine::Quaternion::FromRotationMatrix(m_q.ComputeRotationMatrix());
		PrintQuaternion(m_q);
		PrintQuaternion(q);
		AE_INFO("%f", AstralEngine::Math::CopySign(5.5f, -3.3f));

	}

	void OnUpdate() override
	{
		auto dir = m_target.GetTransform().position - GetTransform().position;
		dir.Normalize();
		m_q = AstralEngine::Quaternion::LookRotation(dir);
		AE_INFO("norm: %f", m_q.Magnitude());
		if (dir == AstralEngine::Vector3::Up() || dir == AstralEngine::Vector3::Down())
		{
			m_target.GetComponent<AstralEngine::SpriteRenderer>().SetColor(1, 0, 0, 1);
		}
		else
		{
			m_target.GetComponent<AstralEngine::SpriteRenderer>().SetColor(0, 1, 0, 1);
		}
		AE_INFO("x: %f, y: %f, z: %f", m_target.GetTransform().position.x, m_target.GetTransform().position.y, m_target.GetTransform().position.z);
	}

	void OnLateUpdate()
	{
		AstralEngine::Vector3 pos[] = {
			AstralEngine::Vector3::Zero(),
			AstralEngine::Vector3(0, 0, 0.1f),
			m_q * AstralEngine::Vector3::Forward() * m_target.GetTransform().position.Magnitude()
		};

		AstralEngine::Vector3 normal[] = {
			{0, 0, 1},
			{0, 0, 1},
			{0, 0, 1}
		};

		unsigned int indices[] = {
			0, 1, 2
		};

		AstralEngine::Vector3 texCoords[] = {
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0}
		};


		AstralEngine::Renderer::BeginScene(m_camera.GetComponent<AstralEngine::Camera>().camera, m_camera.GetTransform());
		AstralEngine::Renderer::DrawQuad(m_q.ComputeRotationMatrix());
		
		AstralEngine::Renderer::DrawVertexData(AstralEngine::RenderingPrimitive::Triangles,
			AstralEngine::Mat4::Identity(), (const AstralEngine::Vector3*)pos, (unsigned int)3, 
			(const AstralEngine::Vector3*)normal, (unsigned int*)indices, (unsigned int)3,
			AstralEngine::Renderer::GetDefaultWhiteTexture(),
			(const AstralEngine::Vector3*)texCoords, 1.0f, 
			(const AstralEngine::Vector4)AstralEngine::Vector4{1, 0, 1, 1});
		
		AstralEngine::Renderer::EndScene();
	}

	void SetCam(AstralEngine::AEntity cam)
	{
		m_camera = cam;
	}

	void SetTarget(AstralEngine::AEntity e)
	{
		m_target = e;
	}

private:
	void PrintQuaternion(const AstralEngine::Quaternion& q)
	{
		AE_INFO("w: %f, x: %f, y: %f, z: %f", q.GetW(), q.GetX(), q.GetY(), q.GetZ());
	}

	AstralEngine::Quaternion m_q;
	AstralEngine::AEntity m_camera;
	AstralEngine::AEntity m_target;

};

class CamControls : public AstralEngine::NativeScript
{
public:
	void OnUpdate()
	{
		Move();
		Rotate();

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::R))
		{
			Reset();
		}
	}

private:
	void Move()
	{
		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::W))
		{
			GetTransform().position.z -= m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::S))
		{
			GetTransform().position.z += m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::A))
		{
			GetTransform().position.x -= m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::D))
		{
			GetTransform().position.x += m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::Space))
		{
			GetTransform().position.y += m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftShift))
		{
			GetTransform().position.y -= m_moveSpeed * AstralEngine::Time::GetDeltaTime();
		}
	}
	void Rotate()
	{
		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftArrow))
		{
			GetTransform().rotation.y += m_rotSpeed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::RightArrow))
		{
			GetTransform().rotation.y -= m_rotSpeed * AstralEngine::Time::GetDeltaTime();
		}
	}

	void Reset()
	{
		GetTransform().position = AstralEngine::Vector3(0.0f, 0.0f, 8.0f);
		GetTransform().rotation = AstralEngine::Vector3(0.0f, 0.0f, 0.0f);
	}

	float m_moveSpeed = 5.0f;
	float m_rotSpeed = 3.0f;
};

class TargetMover : public AstralEngine::NativeScript
{
public:
	void OnCreate()
	{
		GetTransform().position = AstralEngine::Vector3(
			AstralEngine::Random::GetFloat() * (m_bounds.x * 2.0f) - m_bounds.x,
			AstralEngine::Random::GetFloat() * (m_bounds.y * 2.0f) - m_bounds.y,
			AstralEngine::Random::GetFloat() * (m_bounds.z * 2.0f) - m_bounds.z
		);
	}

	void OnUpdate()
	{
		GetTransform().position += m_speed * m_velocity * AstralEngine::Time::GetDeltaTime();
		CheckBounds();
	}

private:
	void CheckBounds()
	{
		if (GetTransform().position.x > m_bounds.x)
		{
			GetTransform().position.x = m_bounds.x;
			m_velocity.x *= -1.0f;
		}

		if (GetTransform().position.x < -m_bounds.x)
		{
			GetTransform().position.x = -m_bounds.x;
			m_velocity.x *= -1.0f;
		}

		if (GetTransform().position.y > m_bounds.y)
		{
			GetTransform().position.y = m_bounds.y;
			m_velocity.y *= -1.0f;
		}

		if (GetTransform().position.y < -m_bounds.y)
		{
			GetTransform().position.y = -m_bounds.y;
			m_velocity.y *= -1.0f;
		}

		if (GetTransform().position.z > m_bounds.z)
		{
			GetTransform().position.z = m_bounds.z;
			m_velocity.z *= -1.0f;
		}

		if (GetTransform().position.z < -m_bounds.z)
		{
			GetTransform().position.z = -m_bounds.z;
			m_velocity.z *= -1.0f;
		}
	}

	AstralEngine::Vector3 m_bounds = AstralEngine::Vector3(5.0f, 5.0f, 5.0f);
	AstralEngine::Vector3 m_velocity = AstralEngine::Vector3(1.0f, 0.5f, 0.2f);
	float m_speed = 2.0f;
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
		m_entity.EmplaceComponent<TargetMover>();

		AstralEngine::AEntity cam = AstralEngine::AEntity((AstralEngine::BaseEntity)0, m_scene.Get());
		m_scene->DestroyAEntity(cam);
		cam = m_scene->CreateAEntity();
		cam.GetTransform().position = AstralEngine::Vector3(0, 0, 8);
		cam.EmplaceComponent<CamControls>();
		auto& camComponent = cam.EmplaceComponent<AstralEngine::Camera>();
		camComponent.primary = true;
		camComponent.camera.SetProjectionType(AstralEngine::SceneCamera::ProjectionType::Perspective);
		
		AstralEngine::AEntity quaternionTester = m_scene->CreateAEntity();
		QuaternionTester& q = quaternionTester.EmplaceComponent<QuaternionTester>();
		q.SetCam(cam);
		q.SetTarget(m_entity);
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