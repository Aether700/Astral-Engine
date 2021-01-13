#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//Scripts////////////////////////////////////////////////////////////////////////


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
			curr += AstralEngine::Time::DeltaTime();
		}
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
		sprite = &GetComponent<AstralEngine::SpriteRendererComponent>();
		test = &GetComponent<TestComponent>();

		curr = 0.0f;
		delay = 0.3f;
	}

	void OnUpdate() override
	{
		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::C))
		{
			sprite->SetColor(0, 1, 0, 1);
		}
		else
		{
			sprite->SetColor(1, 0, 0, 1);
		}

		if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::X))
		{
			Destroy(entity);
		}
		else if (AstralEngine::Input::IsKeyPressed(AstralEngine::KeyCode::K) && curr >= delay)
		{
			test->SetEnable(!test->IsEnabled());
			curr = 0.0f;
		}
		
		if (curr < delay)
		{
			curr += AstralEngine::Time::DeltaTime();
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
	AstralEngine::SpriteRendererComponent* sprite;
	TestComponent* test;
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
			curr += AstralEngine::Time::DeltaTime();
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

		transform->position = { 5, 0, 0 };
		curr = timer;
	}

	void OnUpdate() override
	{
		if (curr < timer)
		{
			curr += AstralEngine::Time::DeltaTime();
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
			curr += AstralEngine::Time::DeltaTime();
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
			curr += AstralEngine::Time::DeltaTime();
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

//layer/////////////////////////////////

class TestLayer : public AstralEngine::Layer
{
public:

	void OnAttach() override
	{
		AstralEngine::AWindow* window = AstralEngine::Application::GetApp()->GetWindow();
		unsigned int width = window->GetWidth(), height = window->GetHeight();
		float aspectRatio = (float)width / (float)height;
		m_cameraController = AstralEngine::AReference<AstralEngine::OrthographicCameraController>::Create(aspectRatio, true);
		m_cameraController->SetZoomLevel(5.5f);

		m_texture = AstralEngine::Texture2D::Create("assets/textures/septicHanzo.png");

		//m_framebuffer = AstralEngine::Framebuffer::Create(width, height);

		m_scene = AstralEngine::AReference<AstralEngine::Scene>::Create();

		m_entity = m_scene->CreateAEntity();
		
		m_entity.EmplaceComponent<AstralEngine::SpriteRendererComponent>(1, 0, 0, 1);
		m_entity.EmplaceComponent<ColorSwitcher>();
		
		m_entity.EmplaceComponent<EntityController>();
		m_entity.EmplaceComponent<TestComponent>();
		
		AstralEngine::AEntity spriteRemover = m_scene->CreateAEntity();
		spriteRemover.EmplaceComponent<SpriteRemover>();

		AstralEngine::AEntity doubleSprite = m_scene->CreateAEntity();
		
		doubleSprite.GetComponent<AstralEngine::TransformComponent>().position.x = -5.0f;
		doubleSprite.EmplaceComponent<SpriteStack>();

		m_scene->CreateAEntity().EmplaceComponent<MultiComponentController>();

	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();

		AstralEngine::Renderer2D::ResetStats();
	}

	bool OnEvent(AstralEngine::AEvent& e) override
	{
		m_cameraController->OnEvent(e);
		return false;
	}

private:
	AstralEngine::AReference<AstralEngine::OrthographicCameraController> m_cameraController;
	AstralEngine::AReference<AstralEngine::Texture2D> m_texture;
	//AstralEngine::AReference<AstralEngine::Framebuffer> m_framebuffer;
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