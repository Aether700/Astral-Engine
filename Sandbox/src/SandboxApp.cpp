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
class ASCIIFontTester : public NativeScript
{
public:
	void OnStart() override
	{
		if (!HasComponent<SpriteRenderer>())
		{
			EmplaceComponent<SpriteRenderer>();
		}

		GetTransform().SetScale({ 5, 5, 1 });
		/*
		AEntity child = CreateAEntity();
		child.GetTransform().SetLocalPosition({3, 0, 0});
		child.GetTransform().SetParent(GetAEntity());
		SpriteRenderer& renderer = child.EmplaceComponent<SpriteRenderer>();
		renderer.SetSprite(Texture2D::WhiteTexture());

		child = CreateAEntity();
		child.GetTransform().SetLocalPosition({ -3, 0, 0 });
		child.GetTransform().SetParent(GetAEntity());
		SpriteRenderer& renderer2 = child.EmplaceComponent<SpriteRenderer>();
		renderer2.SetSprite(Texture2D::WhiteTexture());
		*/
	}

	void OnUpdate() override
	{
		HandleInput();
	}

	//check to test compound glyphs

	void SetFont(AReference<TTFFont> font) 
	{
		m_font = font;
		m_font->SetResolution(s_resolution);
		UpdateRenderData();
	}

private:
	void HandleInput() 
	{
		if (Input::GetKeyDown(KeyCode::I)) 
		{
			m_currChar = m_currChar + 1;
			if (m_currChar > s_max)
			{
				m_currChar = s_min;
			}
			UpdateRenderData();
		}

		if (Input::GetKeyDown(KeyCode::K))
		{
			m_currChar = m_currChar - 1;
			if (m_currChar < s_min)
			{
				m_currChar = s_max;
			}
			UpdateRenderData();
		}

		if (Input::GetKeyDown(KeyCode::L))
		{
			static size_t resolution = s_resolution;
			resolution++;
			m_font->SetResolution(resolution);
			UpdateRenderData();
		}
	}

	void UpdateRenderData()
	{
		m_charTexture = m_font->GetCharTexture(m_currChar);
		if (HasComponent<SpriteRenderer>())
		{
			GetComponent<SpriteRenderer>().SetSprite(m_charTexture);
			//GetComponent<SpriteRenderer>().SetSprite(Texture2D::WhiteTexture());
		}
	}

	static constexpr int s_min = 33;
	static constexpr int s_max = 126;
	static constexpr size_t s_resolution = 15;
	char m_currChar = s_min;
	AReference<TTFFont> m_font;
	Texture2DHandle m_charTexture;
	Transform m_transform = Transform(Vector3::Zero(), Quaternion::Identity(), Vector3(0.0001f, 0.0001f, 1));
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
		ASCIIFontTester& asciiTester = m_entity.EmplaceComponent<ASCIIFontTester>();
		asciiTester.OnStart();
		asciiTester.SetFont(AstralEngine::TTFFont::LoadFont("assets/fonts/arial.ttf"));
	}

	void OnUpdate() override
	{
		//m_scene->OnUpdate();
		AWindow* window = Application::GetWindow();
		m_scene->OnViewportResize(window->GetWidth(), window->GetHeight());


		RenderCommand::SetClearColor(0.1, 0.1, 0.1, 1);
		RenderCommand::Clear();
		Renderer::BeginScene(Camera::GetMainCamera().GetComponent<Camera>(), Camera::GetMainCamera().GetTransform());
		m_scene->OnUpdate();
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