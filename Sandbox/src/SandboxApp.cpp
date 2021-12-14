#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
////////

//Scripts////////////////////////////////////////////////////////////////////////
class PerlinNoiseTest : public AstralEngine::NativeScript
{
public:
	void OnCreate() override
	{
		GenerateTexture();
		entity.EmplaceComponent<AstralEngine::SpriteRendererComponent>(m_texture);
	}

private:

	void GenerateTexture()
	{
		unsigned char* data = (unsigned char*) malloc(sizeof(unsigned char) * size * size * 4);
		
		for (size_t y = 0; y < size; y++)
		{
			for (size_t x = 0; x < size; x++)
			{
				float c = AstralEngine::Math::PerlinNoise((((float)x / ((float)size))) * scale, 
					(((float)y / ((float)size))) * scale);
				unsigned char result = (unsigned char)(c * 255.0f);
				data[(x + size * y) * 4] = result;
				data[(x + size * y) * 4 + 1] = result;
				data[(x + size * y) * 4 + 2] = result;
				data[(x + size * y) * 4 + 3] = 255;
			}
		}

		m_texture = AstralEngine::Texture2D::Create(size, size, data, size * size * 4);
		delete data;
	}

	unsigned int size = 256;
	float scale = 20.0f;
	AstralEngine::AReference<AstralEngine::Texture2D> m_texture;
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

		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::P) && curr >= delay)
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
		AstralEngine::SpriteRendererComponent& sprite = GetComponent<AstralEngine::SpriteRendererComponent>();
		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::C))
		{
			sprite.SetColor(0, 1, 0, 1);
		}
		else
		{
			sprite.SetColor(1, 0, 0, 1);
		}

		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::X))
		{
			Destroy(entity);
		}
		else if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::K) && curr >= delay)
		{
			TestComponent& test = GetComponent<TestComponent>();
			test.SetEnable(!test.IsEnabled());
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
		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::T) && curr >= delay)
		{
			switcher->SetEnable(!switcher->IsEnabled());
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
		if (!entity.HasComponent<AstralEngine::SpriteRendererComponent>())
		{
			AddSprite();
		}

		GetTransform().position = { 5, 0, 0 };
		curr = timer;
	}

	void OnUpdate() override
	{
		if (curr < timer)
		{
			curr += AstralEngine::Time::GetDeltaTime();
		}

		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::B) && curr >= timer)
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
		AstralEngine::SpriteRendererComponent& sprite = entity.EmplaceComponent<AstralEngine::SpriteRendererComponent>();
		sprite.SetColor(0, 1, 0, 1);
		m_hasSprite = true;
	}

	void RemoveSprite()
	{
		entity.RemoveComponent<AstralEngine::SpriteRendererComponent>();
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
			if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::UpArrow))
			{
				AddSprite();
				curr = 0.0f;
			}
			else if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::DownArrow))
			{
				RemoveSprite();
				curr = 0.0f;
			}
		}
	}
private:

	void AddSprite()
	{
		entity.EmplaceComponent<AstralEngine::SpriteRendererComponent>();
		AE_INFO("Added a sprite");
	}

	void RemoveSprite()
	{
		if (entity.HasComponent<AstralEngine::SpriteRendererComponent>())
		{
			entity.RemoveComponent<AstralEngine::SpriteRendererComponent>();
			AE_INFO("Removed a sprite");
		}
		else
		{
			AE_INFO("entity has no sprite component to remove");
		}
	}

	AstralEngine::AStack<AstralEngine::SpriteRendererComponent> stack;

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
			if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::UpArrow))
			{
				CreateMulti();
				curr = 0.0f;
			}
			else if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::DownArrow))
			{
				DeleteMulti();
				curr = 0.0f;
			}
		}
	}

private:
	void CreateMulti()
	{
		entity.EmplaceComponent<MultiComponent>();
	}

	void DeleteMulti()
	{
		if (!entity.HasComponent<MultiComponent>())
		{
			AE_INFO("Entity has no multi component component");
			return;
		}
		entity.RemoveComponent<MultiComponent>();
	}

	float timer = 0.3f;
	float curr;
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
		
		m_entity.EmplaceComponent<AstralEngine::SpriteRendererComponent>(1, 0, 0, 1);
		m_entity.EmplaceComponent<ColorSwitcher>();
		
		m_entity.EmplaceComponent<EntityController>();
		m_entity.EmplaceComponent<TestComponent>();
		AstralEngine::AEntity spriteRemover = m_scene->CreateAEntity();
		spriteRemover.EmplaceComponent<SpriteRemover>();

		
		AstralEngine::AEntity e = m_scene->CreateAEntity();
		e.EmplaceComponent<PerlinNoiseTest>();
		e.GetComponent<AstralEngine::TransformComponent>().scale.x = 5.0f;
		e.GetComponent<AstralEngine::TransformComponent>().scale.y = 5.0f;
		e.GetComponent<AstralEngine::TransformComponent>().position.y = 5.0f;

		AstralEngine::AEntity texturedQuad = m_scene->CreateAEntity();
		texturedQuad.GetTransform().position.x = -5.0f;
		AstralEngine::SpriteRendererComponent& spriteRenderer 
			= texturedQuad.EmplaceComponent<AstralEngine::SpriteRendererComponent>();
		spriteRenderer.SetSprite(AstralEngine::Texture2D::Create("assets/textures/septicHanzo.PNG"));
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();

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