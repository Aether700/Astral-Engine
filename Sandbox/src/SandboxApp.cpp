#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/Physics/Physics2D/Rigidbody2D.h"
#include "AstralEngine/Physics/Physics2D/Collider2D.h"
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

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftArrow))
		{
			rb.AddTorque(rotationSpeed);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::RightArrow))
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
		
		m_entity.EmplaceComponent<AstralEngine::SpriteRenderer>(1, 0, 0, 1);
		m_entity.EmplaceComponent<AstralEngine::Rigidbody2D>();
		m_entity.EmplaceComponent<AstralEngine::BoxCollider2D>();
		m_entity.EmplaceComponent<PhysicObj>();
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();
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