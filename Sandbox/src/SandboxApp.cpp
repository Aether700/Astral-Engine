#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/UI/UIText.h"
#include "AstralEngine/UI/UI Core/TTF/TTFParser.h"
////////

using namespace AstralEngine;


//Scripts////////////////////////////////////////////////////////////////////////
class InputTest : public AstralEngine::NativeScript
{
public:
	void OnUpdate()
	{
		if (AstralEngine::Input::GetMouseButton(AstralEngine::MouseButtonCode::Right))
		{
			if (m_currTimerVal >= m_timer)
			{
				Destroy(GetAEntity());
			}
			else
			{
				m_currTimerVal += AstralEngine::Time::GetDeltaTime();
				return;
			}
		}
		else if (AstralEngine::Input::GetMouseButtonDown(AstralEngine::MouseButtonCode::Left))
		{
			GetComponent<AstralEngine::SpriteRenderer>().SetColor(1, 0, 0, 1);
			m_currTimerVal = 0.0f;
		}
		else
		{
			GetComponent<AstralEngine::SpriteRenderer>().SetColor(1, 1, 1, 1);
			m_currTimerVal = 0.0f;
		}
	}

private:
	float m_timer = 1.0f;
	float m_currTimerVal = 0.0f;
};

class TestComponent : public AstralEngine::NativeScript
{
public:
	void OnCreate() override
	{
		AE_INFO("Test component created");
	}

	void OnStart() override
	{
		print = false;
		curr = 0.0f;
		delay = 0.3f;
	}

	void OnUpdate() override
	{
		if (print)
		{
			AE_INFO("TestComponent::OnUpdate called");
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::P) && curr >= delay)
		{
			print = !print;
			curr = 0.0f;
		}

		if (curr < delay)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}
	}

	void OnDestroy()
	{
		AE_INFO("TestComponent Destroyed");
	}

	void OnEnable() override
	{
		AE_INFO("TestComponent enabled");
	}

	void OnDisable() override
	{
		AE_INFO("TestComponent disabled");
	}

private:
	bool print;
	float curr;
	float delay;
};

class ColorSwitcher : public AstralEngine::NativeScript
{
public:

	void OnCreate() override 
	{
		AE_WARN("OnCreate called");
	}

	void OnStart() override
	{
		curr = 0.0f;
		delay = 0.3f;
	}

	void OnUpdate() override
	{
		AstralEngine::SpriteRenderer& sprite = GetComponent<AstralEngine::SpriteRenderer>();
		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::C))
		{
			sprite.SetColor(0, 1, 0, 1);
		}
		else
		{
			sprite.SetColor(1, 0, 0, 1);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::X))
		{
			Destroy(GetAEntity());
		}
		else if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::K) && curr >= delay)
		{
			TestComponent& test = GetComponent<TestComponent>();
			test.SetActive(!test.IsActive());
			curr = 0.0f;
		}
		
		if (curr < delay)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}
	}

	void OnDestroy() override
	{
		AE_WARN("Square destroyed");
	}

	void OnEnable() override
	{
		AE_WARN("Enabled");
	}

	void OnDisable() override
	{
		AE_WARN("Disabled");
	}

private:
	float curr;
	float delay;
};

class EntityController : public AstralEngine::NativeScript
{
public:

	void OnStart() override
	{
		switcher = &GetComponent<ColorSwitcher>();
	}

	void OnUpdate() override
	{
		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::T) && curr >= delay)
		{
			switcher->SetActive(!switcher->IsActive());
			curr = 0.0f;
		}

		if (curr < delay)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}
	}

private:
	ColorSwitcher* switcher;
	float curr = 0.0f;
	float delay = 0.5f;
};

class SpriteRemover : public AstralEngine::NativeScript
{
public:

	void OnStart() override
	{
		if (!GetAEntity().HasComponent<AstralEngine::SpriteRenderer>())
		{
			AddSprite();
		}

		GetTransform().SetLocalPosition({5, 0, 0});
		curr = timer;
	}

	void OnUpdate() override
	{
		if (curr < timer)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::B) && curr >= timer)
		{
			ToggleSprite();
			curr = 0.0f;
		}
	}


private:

	void ToggleSprite()
	{
		if (m_hasSprite)
		{
			RemoveSprite();
		}
		else
		{
			AddSprite();
		}
	}

	void AddSprite()
	{
		AstralEngine::SpriteRenderer& sprite = GetAEntity().EmplaceComponent<AstralEngine::SpriteRenderer>();
		sprite.SetColor(0, 1, 0, 1);
		m_hasSprite = true;
	}

	void RemoveSprite()
	{
		GetAEntity().RemoveComponent<AstralEngine::SpriteRenderer>();
		m_hasSprite = false;
	}

	bool m_hasSprite;
	float timer = 0.5f;
	float curr;
};

class SpriteStack : public AstralEngine::NativeScript
{
public:
	void OnUpdate() override
	{
		if (curr < timer)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}

		if (curr >= timer)
		{
			if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::UpArrow))
			{
				AddSprite();
				curr = 0.0f;
			}
			else if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::DownArrow))
			{
				RemoveSprite();
				curr = 0.0f;
			}
		}
	}
private:

	void AddSprite()
	{
		GetAEntity().EmplaceComponent<AstralEngine::SpriteRenderer>();
		AE_INFO("Added a sprite");
	}

	void RemoveSprite()
	{
		if (GetAEntity().HasComponent<AstralEngine::SpriteRenderer>())
		{
			GetAEntity().RemoveComponent<AstralEngine::SpriteRenderer>();
			AE_INFO("Removed a sprite");
		}
		else
		{
			AE_INFO("entity has no sprite component to remove");
		}
	}

	AstralEngine::AStack<AstralEngine::SpriteRenderer> stack;

	float timer = 0.3f;
	float curr;
};

class MultiComponent : public AstralEngine::NativeScript
{
public:
	void OnCreate() override
	{
		AE_INFO("MultiComponent created");
	}

	void OnDestroy() override
	{
		AE_INFO("MultiComponent destroyed");
	}


};

class MultiComponentController : public AstralEngine::NativeScript
{
public:

	void OnStart() //override
	{
		curr = timer;
	}

	void OnUpdate() //override
	{
		if (curr < timer)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}

		if (curr >= timer)
		{
			if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::UpArrow))
			{
				CreateMulti();
				curr = 0.0f;
			}
			else if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::DownArrow))
			{
				DeleteMulti();
				curr = 0.0f;
			}
		}
	}

private:
	void CreateMulti()
	{
		GetAEntity().EmplaceComponent<MultiComponent>();
	}

	void DeleteMulti()
	{
		if (!GetAEntity().HasComponent<MultiComponent>())
		{
			AE_INFO("Entity has no multi component component");
			return;
		}
		GetAEntity().RemoveComponent<MultiComponent>();
	}

	float timer = 0.3f;
	float curr;
};

class DisableSprite : public AstralEngine::NativeScript 
{
public:
	void OnUpdate()
	{
		if (AstralEngine::Input::GetKeyDown(AstralEngine::KeyCode::T))
		{
			AstralEngine::SpriteRenderer& spriteRenderer 
				= GetComponent<AstralEngine::SpriteRenderer>();
			spriteRenderer.SetActive(!spriteRenderer.IsActive());
		}
	}
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

void TestIndividualPower(float num, int power)
{
	float astralResult = Math::Power(num, power);
	float stdResult = std::pow(num, power);

	AE_ASSERT(num == power, "");
}

void TestPower()
{
	TestIndividualPower(2, 2);
	TestIndividualPower(2, 3);
	TestIndividualPower(2, 4);
	TestIndividualPower(5, 2);
	TestIndividualPower(5, 3);
	TestIndividualPower(5.5f, 2);
	TestIndividualPower(5.5f, 9);
	TestIndividualPower(5.5f, -9);
	TestIndividualPower(5.5f, -10);
}

//layer/////////////////////////////////

class TestLayer : public AstralEngine::Layer
{
public:

	void OnAttach() override
	{
		TestPower();
		/*
		AstralEngine::AWindow* window = AstralEngine::Application::GetWindow();
		unsigned int width = window->GetWidth(), height = window->GetHeight();
		float aspectRatio = (float)width / (float)height;
		m_cameraController = AstralEngine::AReference<AstralEngine::OrthographicCameraController>::Create(aspectRatio, true);
		m_cameraController->SetZoomLevel(5.5f);
		*/

		AstralEngine::AReference<AstralEngine::UIWindow> uiWindow 
			= AstralEngine::UIContext::CreateUIWindow({ 100, 100 }, 200, 200, 
				AstralEngine::UIWindowFlags::UIWindowFlagsNone, AstralEngine::Vector4(1, 1, 1, 1));
		AstralEngine::UIContext::CreateUIWindow({ 800, 300 }, 200, 200,
			AstralEngine::UIWindowFlags::UIWindowFlagsNone, AstralEngine::Vector4(1, 1, 1, 1));

		AstralEngine::AReference<AstralEngine::UIButton> button 
			= AstralEngine::AReference<AstralEngine::UIButton>::Create("My Button", AstralEngine::Vector4(0.8f, 0, 0, 1));
		uiWindow->AddElement((AstralEngine::AReference<AstralEngine::RenderableUIElement>)button);
		button->SetParent(uiWindow);
		button->AddListener(AstralEngine::ADelegate<void()>(&OnButtonClicked));
		m_texture = AstralEngine::ResourceHandler::LoadTexture2D("assets/textures/septicHanzo.png");

		AstralEngine::AReference<AstralEngine::UIWindow> textWindow
			= AstralEngine::UIContext::CreateUIWindow({ 300, 500 }, 500, 500,
				AstralEngine::UIWindowFlags::UIWindowFlagsNone,
				AstralEngine::Vector4(1, 0, 0, 1));
		/*
		AstralEngine::AReference<AstralEngine::TextElement> text 
			= AstralEngine::AReference<AstralEngine::TextElement>::Create(
				AstralEngine::Font::Create("assets/fonts/arial.fnt"), "I'm\tStopping   e\thi jim");
		text->SetPadding(5);
		text->SetScreenCoordsWidth(400);
		text->SetScreenCoordsHeight(400);
		text->SetColor(AstralEngine::Vector4(0, 1, 0, 1));
		text->SetParent(textWindow);
		textWindow->AddElement((AstralEngine::AReference<AstralEngine::RenderableUIElement>)text);
		*/

		//m_framebuffer = AstralEngine::Framebuffer::Create(width, height);

		m_scene = AstralEngine::AReference<AstralEngine::Scene>::Create();
		m_entity = m_scene->CreateAEntity();
		
		m_entity.EmplaceComponent<AstralEngine::SpriteRenderer>(1, 0, 0, 1);
		m_entity.EmplaceComponent<ColorSwitcher>();
		
		m_entity.EmplaceComponent<EntityController>();
		m_entity.EmplaceComponent<TestComponent>();
		AstralEngine::AEntity spriteRemover = m_scene->CreateAEntity();
		spriteRemover.EmplaceComponent<SpriteRemover>();

		
		AstralEngine::AEntity e = m_scene->CreateAEntity();
		//auto& sprite = e.EmplaceComponent<AstralEngine::SpriteRenderer>(text->GetFont()->GetFontAtlas());
		e.GetTransform().SetScale({ 5.0f, 5.0f, 1.0f });
		e.GetTransform().SetLocalPosition(e.GetTransform().GetLocalPosition().x, 5.0f, 
			e.GetTransform().GetLocalPosition().z);
		e.EmplaceComponent<DisableSprite>();

		//e.GetComponent<AstralEngine::SpriteRenderer>().SetEnable(false);

		AstralEngine::AEntity texturedQuad = m_scene->CreateAEntity();
		texturedQuad.GetTransform().SetLocalPosition(e.GetTransform().GetLocalPosition() - Vector3::Back() * 0.1f);
		AstralEngine::SpriteRenderer& spriteRenderer 
			= texturedQuad.EmplaceComponent<AstralEngine::SpriteRenderer>();
		spriteRenderer.SetSprite(m_texture);
		texturedQuad.EmplaceComponent<InputTest>();

		//AstralEngine::TTFParser::LoadFont("assets/fonts/The Giant and the Mouse.ttf");
		
		debugFont = (AReference<DebugTTFFont>)AstralEngine::TTFParser::LoadFont("assets/fonts/arial.ttf");

		// temp
		m_scene = AReference<Scene>::Create();
		e = m_scene->CreateAEntity();
		e.EmplaceComponent<SpriteRenderer>(Vector4(0, 1, 0, 1));
		Camera::GetMainCamera().GetComponent<Camera>().GetCamera().
			SetProjectionType(SceneCamera::ProjectionType::Perspective);
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();
		AWindow* window = Application::GetWindow();
		m_scene->OnViewportResize(window->GetWidth(), window->GetHeight());


		RenderCommand::SetClearColor(0.1, 0.1, 0.1, 1);
		RenderCommand::Clear();
		Renderer::BeginScene(Camera::GetMainCamera().GetComponent<Camera>(), Camera::GetMainCamera().GetTransform());
		debugFont->DebugDrawGlyph();
		Renderer::EndScene();

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

	
	AstralEngine::Texture2DHandle m_texture;
	AstralEngine::AReference<AstralEngine::Framebuffer> m_framebuffer;
	AstralEngine::AReference<AstralEngine::Scene> m_scene;
	AstralEngine::AEntity m_entity;

	AReference<DebugTTFFont> debugFont;
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