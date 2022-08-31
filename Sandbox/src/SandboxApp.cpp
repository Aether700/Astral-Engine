#include <AstralEngine.h>
#include <AstralEngine/EntryPoint.h>
#include <iostream>

//temp
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/Renderer/RendererInternals.h"
////////

using namespace AstralEngine;

//Scripts////////////////////////////////////////////////////////////////////////

class Rotator : public NativeScript
{
public:
	Rotator() : m_axis(Vector3::Up()) { }

	void SetAxis(const Vector3& axis) { m_axis = axis; }

	void OnUpdate()
	{
		static bool rotate = false;

		if (Input::GetKeyDown(KeyCode::R))
		{
			rotate = !rotate;
		}

		if (rotate)
		{
			Transform& t = GetTransform();
			Quaternion rotationThisFrame = Quaternion::AngleAxisToQuaternion(m_speed * Time::GetDeltaTime(), m_axis);
			t.SetRotation(rotationThisFrame * t.GetRotation());
		}
	}

private:

	Vector3 m_axis;
	float m_speed = 5.0f;
};

class RendererStatViewer : public NativeScript
{
public:
	void OnStart() override
	{
		m_stat = Stat::FrameRate;
		m_print = false;
	}

	void OnUpdate() override
	{
		if (Input::GetKeyDown(KeyCode::P))
		{
			m_print = !m_print;
		}

		if (m_print)
		{
			if (Input::GetKeyDown(KeyCode::K))
			{
				int i = (int)m_stat;
				i = (i + 1) % (int)Stat::Count;
				m_stat = (Stat)i;
			}

			switch (m_stat)
			{
			case Stat::DrawCalls:
				std::cout << "Num Draw Calls: " << Renderer::GetStats().numDrawCalls << "\n";
				break;

			case Stat::NumIndices:
				std::cout << "Num Indices: " << Renderer::GetStats().numIndices << "\n";
				break;

			case Stat::NumVertices:
				std::cout << "Num Vertices: " << Renderer::GetStats().numVertices << "\n";
				break;

			case Stat::TimePerFrame:
				std::cout << "Time Per frame: " << (float)Renderer::GetStats().timePerFrame << " secs\n";
				break;

			case Stat::FrameRate:
				std::cout << "Frame Rate: " << (float)Renderer::GetStats().GetFrameRate() << "\n";
				break;
			}
		}
	}

private:
	enum class Stat
	{
		DrawCalls,
		NumVertices,
		NumIndices,
		TimePerFrame,
		FrameRate,
		Count
	};

	Stat m_stat;
	bool m_print;
};

class MatToggler : public NativeScript
{
public:
	void OnStart()
	{
		if (HasComponent<MeshRenderer>())
		{
			m_mat = GetComponent<MeshRenderer>().GetMaterial();
		}
	}

	void OnUpdate()
	{
		if (Input::GetKeyDown(KeyCode::T))
		{
			if (HasComponent<MeshRenderer>())
			{
				MeshRenderer& mesh = GetComponent<MeshRenderer>();
				if (mesh.GetMaterial() == NullHandle)
				{
					mesh.SetMaterial(m_mat);
				}
				else
				{
					mesh.SetMaterial(NullHandle);
				}
			}
		}
	}

private:
	MaterialHandle m_mat;
};

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
		Vector3 pos = t.GetLocalPosition();
		Vector3 camPos = Renderer::GetCamPos();

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
			t.SetLocalPosition(t.GetLocalPosition() + t.Up() * m_speed
				* AstralEngine::Time::GetDeltaTime());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::LeftShift))
		{
			t.SetLocalPosition(t.GetLocalPosition() - t.Up() * m_speed 
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

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::UpArrow))
		{
			Quaternion rotation = Quaternion::AngleAxisToQuaternion(-m_rotSpeed * Time::GetDeltaTime(), t.Right());
			t.SetRotation(rotation * t.GetRotation());
		}

		if (AstralEngine::Input::GetKey(AstralEngine::KeyCode::DownArrow))
		{
			Quaternion rotation = Quaternion::AngleAxisToQuaternion(m_rotSpeed * Time::GetDeltaTime(), t.Right());
			t.SetRotation(rotation * t.GetRotation());
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
	ResourceHandler::GetShader(shader)->SetMat4("u_viewProjMatrix", Mat4::Identity());
	vb->Bind();
	ib->Bind();
	RenderCommand::DrawInstancedIndexed(ib, 1);
}

void SetupRenderingData(AReference<VertexBuffer> vb, AReference<VertexBuffer> instanced, AReference<IndexBuffer> ib)
{
	struct TempVertex
	{
		Vector3 pos;
		Vector2 textureCoord;
	};

	TempVertex data[4] =
	{
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } }
	};

	vb->Bind();
	vb->SetLayout({ 
		{ ADataType::Float3, "Position" },
		{ ADataType::Float2, "textureCoords" }
		});
	vb->SetData(data, sizeof(data));

	unsigned int indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	ib->Bind();
	ib->SetData(indices, sizeof(indices) / sizeof(unsigned int));
}

MaterialHandle CreateLightCubeMat()
{
	MaterialHandle mat = ResourceHandler::CreateMaterial();
	auto& material = ResourceHandler::GetMaterial(mat);
	material->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	material->SetShader(ResourceHandler::LoadShader("assets/shaders/DebugCubeLightShader.glsl"));
	return mat;
}

MeshHandle CreateCubeMesh()
{
	ADynArr<Vector3> positions = 
	{
		//back face
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },

		//left face
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f },

		//front face
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },

		//right face
		{  0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f },

		//top face
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },

		//bottom face
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f }
	};
	
	ADynArr<Vector2> textureCoords = 
	{
		//back face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },

		//left face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },

		//front face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },

		//right face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },

		//top face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },

		//bottom face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	ADynArr<Vector3> normals =
	{
		//back face
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		
		//left face
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		
		//front face
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		
		//right face
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		
		//top face
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		
		//bottom face
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f }

	};

	ADynArr<unsigned int> indices =
	{
		//back face
		0, 1, 2,
		2, 3, 0,

		//left face
		4, 5, 6,
		6, 7, 4,

		//front face
		8, 9, 10,
		10, 11, 8,

		//right face
		12, 13, 14,
		14, 15, 12,

		//top face
		16, 17, 18,
		18, 19, 16,

		//bottom face
		20, 21, 22,
		22, 23, 20
	};


	return ResourceHandler::CreateMesh(positions, textureCoords, normals, indices);
}

MaterialHandle CreateMaterial(LightHandle light)
{ 
	MaterialHandle mat = ResourceHandler::CreateMaterial();
	auto& material = ResourceHandler::GetMaterial(mat);
	material->SetColor({ 1, 1, 1, 1 });
	material->SetDiffuseMap(ResourceHandler::LoadTexture2D("assets/textures/crateDiffuse.png"));
	material->SetSpecularMap(ResourceHandler::LoadTexture2D("assets/textures/crateSpecular.png"));
	material->AddUniform(new LightUniform("light", light));
	material->AddUniform(new PrimitiveUniform("u_matShininess", 32.0f));
	material->AddCamPosUniform();
	//material->UseDeferredRendering(true);
	material->UseDeferredRendering(false);
	return mat;
}

//layer/////////////////////////////////

class TestLayer : public AstralEngine::Layer
{
public:

	void OnAttach() override
	{
		// manual rendering
		m_shader = ResourceHandler::LoadShader("assets/shaders/FullscreenTextureShader.glsl");
		AReference<Shader> shader = ResourceHandler::GetShader(m_shader);
		shader->Bind();
		shader->SetInt("u_texture", 0);

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
		//m_entity.EmplaceComponent<AstralEngine::SpriteRenderer>();
		//m_entity.EmplaceComponent<Controller>();
		MeshHandle cube = CreateCubeMesh();
		MaterialHandle lightMat = CreateLightCubeMat();

		auto e = m_scene->CreateAEntity();
		e.GetTransform().SetScale({ 0.2f, 0.2f, 0.2f });
		e.GetTransform().SetLocalPosition({ 3.0f, 5.0f, -2.0f });
		e.EmplaceComponent<MeshRenderer>(cube, lightMat);
		Light& l = e.EmplaceComponent<Light>();
		
		MaterialHandle mat = CreateMaterial(l.GetHandle());

		m_entity.EmplaceComponent<MeshRenderer>(cube, mat);
		
		e = m_scene->CreateAEntity();
		e.EmplaceComponent<MeshRenderer>(cube, mat);
		

		/*
		e.GetTransform().SetLocalPosition({ 2, 0, 0 });
		e.EmplaceComponent<MatToggler>();

		e = m_scene->CreateAEntity();
		e.EmplaceComponent<MeshRenderer>(cube, mat);
		e.GetTransform().SetLocalPosition({ -2, 0, 0 });

		e = m_scene->CreateAEntity();
		e.EmplaceComponent<SpriteRenderer>(ResourceHandler::LoadTexture2D("assets/textures/ChernoLogo.png"));
		e.GetTransform().SetLocalPosition({ 0, 2, 0 });
		
		e = m_scene->CreateAEntity();
		e.EmplaceComponent<SpriteRenderer>(ResourceHandler::LoadTexture2D("assets/textures/septicHanzo.png"));
		e.GetTransform().SetLocalPosition({ 2, 2, 0 });
		*/
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
		cam.EmplaceComponent<RendererStatViewer>();
		//cam.EmplaceComponent<RotateAroundTester>().SetTarget(m_entity);
		m_entity = cam;
		//SetupRendererTestScene();
	}

	void OnUpdate() override
	{
		m_scene->OnUpdate();
		auto* window = AstralEngine::Application::GetWindow();
		m_scene->OnViewportResize(window->GetWidth(), window->GetHeight());

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
	void CreateSprite(const Vector3& pos, Texture2DHandle texture = NullHandle, bool rotator = false)
	{
		auto e = m_scene->CreateAEntity();
		e.GetTransform().SetLocalPosition(pos);
		e.EmplaceComponent<SpriteRenderer>(texture);
		if (rotator)
		{
			e.EmplaceComponent<Rotator>().SetAxis(Vector3::Forward());
		}
	}

	void CreateMesh(const Vector3& pos, const Quaternion& rotation, MeshHandle mesh,
		MaterialHandle mat, bool rotator = false)
	{
		auto e = m_scene->CreateAEntity();
		e.GetTransform().SetLocalPosition(pos);
		e.GetTransform().SetRotation(rotation);
		e.EmplaceComponent<MeshRenderer>(mesh, mat);

		if (rotator)
		{
			e.EmplaceComponent<Rotator>().SetAxis(Vector3::Up());
		}
	}

	void RenderTriangleScene()
	{
		static AReference<VertexBuffer> vb = nullptr;
		static AReference<IndexBuffer> ib = nullptr;
		static AReference<Shader> shader = nullptr;
		if (vb == nullptr)
		{
			Vector2 pos[3] = 
			{
				{ -0.5f, -0.5f },
				{  0.5f, -0.5f },
				{  0.0f,  0.5f }
			};
			vb = VertexBuffer::Create((float*)pos, sizeof(pos));
			vb->SetLayout({ { ADataType::Float2, "pos" } });
			unsigned int indices[] = { 0, 1, 2 };

			ib = IndexBuffer::Create(indices, 3);
			shader = ResourceHandler::GetShader(ResourceHandler::LoadShader("assets/shaders/triangle.glsl"));
		}

		
		shader->Bind();
		vb->Bind();
		ib->Bind();
		RenderCommand::DrawIndexed(ib);
	}

	void SetupRendererTestScene(LightHandle light)
	{
		MeshHandle cube = CreateCubeMesh();
		MaterialHandle mat = CreateMaterial(light);

		Texture2DHandle t = ResourceHandler::LoadTexture2D("assets/textures/ChernoLogo.png");
		Texture2DHandle t2 = ResourceHandler::LoadTexture2D("assets/textures/septicHanzo.png");
		
		constexpr int gridSize = 3;

		// sprites 
		for (int x = -gridSize; x < gridSize; x++)
		{
			for (int y = -gridSize; y < gridSize; y++)
			{
				Texture2DHandle texture = x % 2 == 0 ? t : t2;
				CreateSprite(Vector3(x * 1.25f, y * 1.25f, 3), texture, (x + y) % 3 == 1);
			}
		}

		// mesh
		for (int x = -gridSize; x < gridSize; x++)
		{
			for (int y = -gridSize; y < gridSize; y++)
			{
				CreateMesh(Vector3(x * 1.25f, y * 1.25f, 0), Quaternion::Identity(), cube, mat, (x + y) % 3 == 0);
			}
		}
		/*
		*/
	}

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