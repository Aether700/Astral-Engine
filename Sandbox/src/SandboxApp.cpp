#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/Physics/Physics2D/Rigidbody2D.h"
#include "AstralEngine/Physics/Physics2D/Collider2D.h"
#include "AstralEngine/Physics/Physics2D/CollisionHelper.h"

////////

//Scripts////////////////////////////////////////////////////////////////////////
class PhysicObj : public AstralEngine::NativeScript
{
public:
	void OnUpdate()
	{
		auto& rb = GetRigidbody();

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::Space))
		{
			rb.AddForce(AstralEngine::Vector2(0.0f, verticalForce));
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::L))
		{
			rb.AddTorque(rotationSpeed);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::J))
		{
			rb.AddTorque(-rotationSpeed);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::R))
		{
			ResetObj(rb);
		}

		if (AstralEngine::Input::GetKeyDown(AstralEngine::KeyCode::T))
		{
			ToggleRotation(rb);
		}

		if (AstralEngine::Input::GetKeyDown(AstralEngine::KeyCode::Y))
		{
			ToggleTranslation(rb);
		}
	}

private:
	void ResetObj(AstralEngine::Rigidbody2D& rb)
	{
		rb.SetPosition(AstralEngine::Vector2::Zero());
		rb.SetVelocity(AstralEngine::Vector2::Zero());
		rb.SetRotation(0.0f);
		rb.SetAngularVelocity(0.0f);
	}

	void ToggleRotation(AstralEngine::Rigidbody2D& rb)
	{
		if (rb.GetContraints() & AstralEngine::Rigidbody2DConstraintsFixedRotation)
		{
			rb.SetConstraints(~AstralEngine::Rigidbody2DConstraintsFixedRotation & rb.GetContraints());
		}
		else
		{
			rb.SetConstraints(rb.GetContraints() | AstralEngine::Rigidbody2DConstraintsFixedRotation);
		}
	}

	void ToggleTranslation(AstralEngine::Rigidbody2D& rb)
	{
		if (rb.GetContraints() & AstralEngine::Rigidbody2DConstraintsFixedPosition)
		{
			rb.SetConstraints(~AstralEngine::Rigidbody2DConstraintsFixedPosition & rb.GetContraints());
		}
		else
		{
			rb.SetConstraints(rb.GetContraints() | AstralEngine::Rigidbody2DConstraintsFixedPosition);
		}
	}

	AstralEngine::Rigidbody2D& GetRigidbody()
	{
		return GetComponent<AstralEngine::Rigidbody2D>();
	}

	float verticalForce = 20.0f;
	float rotationSpeed = 0.5f;
};

class BasicMoveScript : public AstralEngine::NativeScript
{
public:
	void OnUpdate() override
	{
		AstralEngine::Transform& t = GetTransform();
		AstralEngine::Vector2 pos = t.GetLocalPosition();
		AstralEngine::Quaternion rotation = t.GetRotation();

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::J))
		{
			pos.x -= m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::L))
		{
			pos.x += m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::I))
		{
			pos.y += m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::K))
		{
			pos.y -= m_speed * AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::U))
		{
			rotation = AstralEngine::Quaternion::EulerToQuaternion(0, 0, m_rotationSpeed * AstralEngine::Time::GetDeltaTime()) * rotation;
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::O))
		{
			rotation = AstralEngine::Quaternion::EulerToQuaternion(0, 0, -m_rotationSpeed * AstralEngine::Time::GetDeltaTime()) * rotation;
		}

		t.SetLocalPosition(pos);
		t.SetRotation(rotation);

		AstralEngine::SpriteRenderer& renderer = GetComponent<AstralEngine::SpriteRenderer>();
		if (AstralEngine::CollisionHelper::BoxToBoxCollision(
			m_other.GetComponent<AstralEngine::BoxCollider2D>(), 
			GetComponent<AstralEngine::BoxCollider2D>()))
		{
			renderer.SetColor(0, 1, 0, 1);
		}
		else
		{
			renderer.SetColor(1, 0, 0, 1);
		}
	}

	void SetSecondEntity(AstralEngine::AEntity other) { m_other = other; }

private:
	float m_speed = 5.0f;
	float m_rotationSpeed = 20.0f;
	AstralEngine::AEntity m_other;
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
		
		AstralEngine::Camera& cam = AstralEngine::Camera::GetMainCamera().GetComponent<AstralEngine::Camera>();
		cam.GetCamera().SetOrthographicFarClip(20.0f);

		m_entity.EmplaceComponent<AstralEngine::SpriteRenderer>(1, 0, 0, 1);
		//m_entity.EmplaceComponent<AstralEngine::Rigidbody2D>();
		m_entity.EmplaceComponent<AstralEngine::BoxCollider2D>();
		BasicMoveScript& script = m_entity.EmplaceComponent<BasicMoveScript>();
		m_entity.GetTransform().SetLocalPosition(3, 0, 0);

		AstralEngine::AEntity e = m_scene->CreateAEntity();
		e.EmplaceComponent<AstralEngine::SpriteRenderer>(0, 0, 1, 1);
		e.EmplaceComponent<AstralEngine::BoxCollider2D>();

		script.SetSecondEntity(e);
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