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
class TessellationTester : public NativeScript
{
public:
	void OnStart()
	{
		//write test to verify tesselation of glyphs
		m_transform = Transform(Vector3::Zero(), Vector3::Zero(), { 1, 1, 1 });

		m_initialPoints.AddLast(Vector2(-5, -2.5));
		m_initialPoints.AddLast(Vector2(-5, 2.5));
		m_initialPoints.AddLast(Vector2(5, 2.5));
		m_initialPoints.AddLast(Vector2(5, -2.5));

		ADoublyLinkedList<ADynArr<Vector2>> listOfPointRings;
		listOfPointRings.AddLast(m_initialPoints);
		listOfPointRings.AddLast({ Vector2(-2.5f, -0.5f), Vector2(-1.5f, -0.5f), Vector2(-2.0f, 1.5f) });
		listOfPointRings.AddLast({ Vector2(2.5f, 1.5f), Vector2(1.5f, 1.5f), Vector2(2.0f, -0.5f) });
		listOfPointRings.AddLast({ Vector2(0.5f, 0.0f), Vector2(-0.5f, 0.5f), Vector2(-0.5f, -0.5f) });
		m_mesh = Tessellation::EarClipping(listOfPointRings, TessellationWindingOrder::ClockWise);
		AE_CORE_ASSERT(m_mesh != NullHandle, "Mesh Generation for glyph Failed");
		m_tessellationPoints = ResourceHandler::GetMesh(m_mesh)->GetPositions();
	}

	void OnUpdate()
	{
		if (Input::GetKeyDown(KeyCode::L))
		{
			m_pointView = !m_pointView;
		}

		if (m_pointView)
		{
			Vector3 scale = { 0.5f, 0.5f, 0.5f };

			for (Vector2& point : m_initialPoints)
			{
				Renderer::DrawQuad(Vector3(point.x, point.y, 1.0f), scale, { 1, 0, 0, 1 });
			}

			for (Vector3& point : m_tessellationPoints)
			{
				Renderer::DrawQuad(point, scale, { 0, 1, 0, 1 });
			}
		}
		else
		{
			Renderer::DrawMesh(m_transform, Material::GlyphMat(), m_mesh);
		}
	}

private:
	ADynArr<Vector2> m_initialPoints;
	ADynArr<Vector3> m_tessellationPoints;
	MeshHandle m_mesh;
	Transform m_transform;
	bool m_pointView = false;
};

class FontViewer : public NativeScript
{
public:
	void OnUpdate() override
	{
		if (m_font == nullptr)
		{
			AE_CORE_ERROR("Did not set font check in TestLayer::OnAttached");
		}

		if (m_mesh != NullHandle)
		{
			Renderer::DrawMesh(m_transform, Material::GlyphMat(), m_mesh);
		}
		CheckUserInputForCharacter();
	}

	void SetFont(AReference<TTFFont> f)
	{
		m_font = f;
		m_mesh = m_font->GetCharMesh(c);
	}

private:
	void CheckUserInputForCharacter()
	{
		if (Input::GetKeyDown(KeyCode::P))
		{
			if (isShifted)
			{
				c += shiftOffset;
			}
			else
			{
				c -= shiftOffset;
			}
			isShifted = !isShifted;
			ResetResolution();
			m_mesh = m_font->GetCharMesh(c);
		}

		if (Input::GetKeyDown(KeyCode::Tab))
		{
			index = (index + 1) % 26;
			c = index + 65;
			if (!isShifted)
			{
				c += shiftOffset;
			}
			ResetResolution();
			m_mesh = m_font->GetCharMesh(c);
		}

		if (Input::GetKeyDown(KeyCode::I))
		{
			resolution = Math::Clamp(resolution + 1, 0, maxResolution);
			m_mesh = m_font->GetCharMesh(c);
			AE_INFO("resolution: %d", resolution);
		}

		if (Input::GetKeyDown(KeyCode::K))
		{
			resolution = Math::Clamp(resolution - 1, 0, maxResolution);
			m_mesh = m_font->GetCharMesh(c);
			AE_INFO("resolution: %d", resolution);
		}
	}

	void ResetResolution()
	{
		resolution = 0;
		AE_INFO("Resolution reset to 0");
	}

	static constexpr int shiftOffset = 'a' - 'A';
	static constexpr int maxResolution = 100;
	AReference<TTFFont> m_font;
	Transform m_transform = Transform(Vector3::Zero(), Quaternion::Identity(), Vector3(0.0001f, 0.0001f, 1));
	char c = 'i';//'A';
	int index = 0;
	bool isShifted = true;
	int resolution = 0;
	MeshHandle m_mesh;
};

class InputTest : public AstralEngine::NativeScript
{
public:
	void OnUpdate()
	{
		if (AstralEngine::Input::GetMouseButton(AstralEngine::MouseButtonCode::Right))
		{
			if (m_currTimerVal >= m_timer)
			{
				DestroyAEntity(GetAEntity());
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
		
		viewer.SetFont(AstralEngine::TTFFont::LoadFont("assets/fonts/arial.ttf"));

		// temp
		AstralEngine::AEntity e = m_scene->CreateAEntity();
		e.EmplaceComponent<SpriteRenderer>(Vector4(0, 1, 0, 1));
		Camera::GetMainCamera().GetComponent<Camera>().GetCamera().
			SetProjectionType(SceneCamera::ProjectionType::Perspective);

		tesTester.OnStart();
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();
		AWindow* window = Application::GetWindow();
		m_scene->OnViewportResize(window->GetWidth(), window->GetHeight());


		RenderCommand::SetClearColor(0.1, 0.1, 0.1, 1);
		RenderCommand::Clear();
		Renderer::BeginScene(Camera::GetMainCamera().GetComponent<Camera>(), Camera::GetMainCamera().GetTransform());
		viewer.OnUpdate();
		//tesTester.OnUpdate();
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

	FontViewer viewer;
	TessellationTester tesTester;
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