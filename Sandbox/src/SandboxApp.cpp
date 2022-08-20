#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
#include "glad/glad.h"
////////

using namespace AstralEngine;

//Scripts////////////////////////////////////////////////////////////////////////

class CamController : public AstralEngine::NativeScript
{
public:
	void OnStart() override
	{
		m_startPos = GetTransform().GetLocalPosition();
	}
	
	void OnUpdate() override
	{
		auto& t = GetTransform();

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::W))
		{
			t.SetLocalPosition(t.GetLocalPosition() + t.Forward() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::S))
		{
			t.SetLocalPosition(t.GetLocalPosition() - t.Forward() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::D))
		{
			t.SetLocalPosition(t.GetLocalPosition() + t.Right() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::A))
		{
			t.SetLocalPosition(t.GetLocalPosition() - t.Right() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::Space))
		{
			t.SetLocalPosition(t.GetLocalPosition() + AstralEngine::Vector3::Up() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftShift))
		{
			t.SetLocalPosition(t.GetLocalPosition() - AstralEngine::Vector3::Up() * m_speed 
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::RightArrow))
		{
			auto euler = t.GetRotation().EulerAngles();
			euler.y += m_rotSpeed * AstralEngine::Time::GetDeltaTime();
			t.SetRotation(euler);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftArrow))
		{
			auto euler = t.GetRotation().EulerAngles();
			euler.y -= m_rotSpeed * AstralEngine::Time::GetDeltaTime();
			t.SetRotation(euler);
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::R))
		{
			t.SetLocalPosition(m_startPos);
			t.SetRotation(AstralEngine::Quaternion::Identity());
		}

	}

private:
	void PrintEuler(const AstralEngine::Vector3& euler)
	{
		AE_INFO("x: %f  y: %f  z: %f", euler.x, euler.y, euler.z);
	}

	float m_speed = 5.0f;
	float m_rotSpeed = 5 * 30.0f;

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
			t.SetLocalPosition(t.GetLocalPosition() + AstralEngine::Vector3::Up()
				* speed * AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::S))
		{
			t.SetLocalPosition(t.GetLocalPosition() - AstralEngine::Vector3::Up()
				* speed * AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::D))
		{
			t.SetLocalPosition(t.GetLocalPosition() + AstralEngine::Vector3::Right()
				* speed * AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::A))
		{
			t.SetLocalPosition(t.GetLocalPosition() - AstralEngine::Vector3::Right()
				* speed * AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKeyDown(AstralEngine::KeyCode::R))
		{
			t.SetLocalPosition(AstralEngine::Vector3::Zero());
		}

		if (AstralEngine::Input::GetMouseButton(AstralEngine::MouseButtonCode::Left))
		{
			AstralEngine::Vector2Int pos = AstralEngine::Input::GetMousePosition();
			AE_WARN("Left mouse button is down. Mouse Pos: %d, %d", pos.x, pos.y);
		}
	}

private:

	float speed = 10.0f;//0.05f;
};

class RotateAroundTester : public AstralEngine::NativeScript
{
public:
	void OnUpdate() override
	{
		if (m_target.IsValid())
		{
			auto& t = GetTransform();
			auto& targetTransform = m_target.GetTransform();
			float angle = m_rotationSpeed * AstralEngine::Time::GetDeltaTime();

			t.RotateAround(targetTransform.GetLocalPosition(), angle, AstralEngine::Vector3::Up());
		}
	}


	void SetTarget(AstralEngine::AEntity e)
	{
		m_target = e;
	}
private:
	AstralEngine::AEntity m_target;
	float m_rotationSpeed = 30.0f;
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

void ManualRender(ShaderHandle shader, AReference<VertexBuffer> vb, AReference<VertexBuffer> instanced, 
	AReference<IndexBuffer> ib)
{
	RenderCommand::Clear();
	ResourceHandler::GetShader(shader)->Bind();
	vb->Bind();
	RenderCommand::DrawInstancedIndexed(ib, 1);
}

void SetupRenderingData(AReference<VertexBuffer> vb, AReference<VertexBuffer> instanced, AReference<IndexBuffer> ib)
{
	Mat4 offsets[1] =
	{
		Mat4::Identity()
	};

	Vector3 pos[4] =
	{
		{ -0.2f, -0.2f, 0.0f },
		{  0.2f, -0.2f, 0.0f },
		{  0.2f,  0.2f, 0.0f },
		{ -0.2f,  0.2f, 0.0f }
	};
	vb->Bind();
	vb->SetLayout({ { ADataType::Float3, "Position" } });
	vb->SetData(pos, sizeof(pos));
	
	instanced->Bind();

	//issue might be with set layout double check to make sure
	//instanced->SetLayout({ { ADataType::Float3, "offsets", false, 1 } }, 1, sizeof(Vector3)); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (const void*)0);
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (const void*)(4 * sizeof(float)));
	glVertexAttribDivisor(2, 1);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (const void*)(8 * sizeof(float)));
	glVertexAttribDivisor(3, 1);
	
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (const void*)(12 * sizeof(float)));
	glVertexAttribDivisor(4, 1);


	instanced->SetData(offsets, sizeof(offsets));

	unsigned int indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	ib->Bind();
	ib->SetData(indices, sizeof(indices) / sizeof(unsigned int));
}

//layer/////////////////////////////////

class TestLayer : public AstralEngine::Layer
{
public:

	void OnAttach() override
	{
		// manual rendering
		m_shader = ResourceHandler::LoadShader("assets/shaders/InstanceRenderingShader.glsl");
		m_vb = VertexBuffer::Create(sizeof(float) * 20);
		m_instancedVB = VertexBuffer::Create(sizeof(float) * 20, true);
		m_ib = IndexBuffer::Create();

		SetupRenderingData(m_vb, m_instancedVB, m_ib);
		//////////////////

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
		
		/*
		auto e = m_scene->CreateAEntity();
		e.EmplaceComponent<TargetMover>();
		e.EmplaceComponent<AstralEngine::SpriteRenderer>(1, 0, 1, 1);
		*/

		//m_entity.EmplaceComponent<LookAtTester>().SetTarget(e);

		AstralEngine::AEntity cam = AstralEngine::AEntity((AstralEngine::BaseEntity)0, m_scene.Get());
		//cam.GetComponent<AstralEngine::Camera>().camera.SetProjectionType(AstralEngine::SceneCamera::ProjectionType::Perspective);
		m_scene->DestroyAEntity(cam);
		cam = m_scene->CreateAEntity();
		cam.EmplaceComponent<AstralEngine::Camera>().camera.SetProjectionType(AstralEngine::SceneCamera::ProjectionType::Perspective);
		cam.GetTransform().SetLocalPosition(0.0f, 0.0f, -8.0f);
		cam.EmplaceComponent<CamController>();
		//cam.EmplaceComponent<RotateAroundTester>().SetTarget(m_entity);
		m_entity = cam;
	}

	void OnUpdate() override
	{
		//ManualRender(m_shader, m_vb, m_instancedVB, m_ib);

		/*
		Renderer::BeginScene(m_entity.GetComponent<Camera>().camera);
		Renderer::DrawQuad(Mat4::Identity()); //nothing drawing
		Renderer::EndScene();
		*/

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
	
	bool OnEvent(AstralEngine::AEvent& e) override
	{
		/*
		using E = AstralEngine::MouseButtonPressedEvent;
		E* eventPtr = dynamic_cast<E*>(&e);

		if (eventPtr != nullptr)
		{
			static int count = 0;
			if (eventPtr->GetButtonKeycode() == AstralEngine::MouseButtonCode::Left)
			{
				count++;
				if (count == 3)
				{
					AstralEngine::Application::Exit();
				}
			}
			else
			{
				count--;
			}
		}
		*/
		return false;
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

	// for manual rendering
	ShaderHandle m_shader;
	AReference<VertexBuffer> m_vb;
	AReference<VertexBuffer> m_instancedVB;
	AReference<IndexBuffer> m_ib;
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