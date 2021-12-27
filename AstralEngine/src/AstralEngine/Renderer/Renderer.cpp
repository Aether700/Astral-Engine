#include "aepch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "ECS/Components.h"
#include "Mesh.h"
#include "AstralEngine/UI/UICore.h"

namespace AstralEngine
{

	AUnorderedMap<RenderingPrimitive, RenderingBatch>* Renderer::s_renderingBatches;
	RendererStatistics Renderer::s_stats;

	AReference<CubeMap> Renderer::s_defaultWhiteCubeMap;
	AReference<Texture2D> Renderer::s_defaultWhiteTexture;
	AReference<Shader> Renderer::s_shader;
	ADynArr<DrawCommand> Renderer::s_drawCommands;
	Material Renderer::s_defaultMaterial = Material(); //create the default material
	bool Renderer::s_useShadows = false;
	DirectionalLight* Renderer::s_directionalLightArr;
	unsigned int Renderer::s_directionalLightIndex;
	PointLight* Renderer::s_pointLightArr;
	unsigned int Renderer::s_pointLightIndex;
	bool Renderer::s_updateLights;

	unsigned int RenderingBatch::s_maxVertices = 64000;
	unsigned int RenderingBatch::s_maxIndices = 72000;
	unsigned int RenderingBatch::s_maxTextureSlots = 32;
	unsigned int RenderingBatch::s_maxTexture2DShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxTexture2DSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxCubemapSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxCubemapShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;

	// Draw Command ////////////////////////////////////////////////////////////////////

	//helper function which adds the provided texture to the provided RenderingBatch 
	//according to the Texture Type and returns the texture index
	template<typename TextureType>
	int AddTextureToBatch(RenderingBatch& batch, RenderingPrimitive renderingTarget, AReference<TextureType>& texture)
	{
		if constexpr (std::is_base_of_v<TextureType, Texture2D>)
		{
			return batch.AddTexture2D(texture, renderingTarget);
		}
		return batch.AddCubemap(texture, renderingTarget);
	}

	class DrawCommand
	{
	public:
		DrawCommand() : m_positionArr(nullptr), m_normalArr(nullptr), 
			m_indicesArr(nullptr), m_textureCoords(nullptr) { }

		DrawCommand(const Mat4& transform, const Material& mat,
			const Vector3* vertices, const Vector3* normals, unsigned int numVertices, const unsigned int* indices,
			unsigned int indexCount, AReference<Texture> texture, const Vector3* textureCoords,
			float tileFactor, const Vector4& tintColor, bool ignoresCam) : m_transform(transform), m_mat(mat), 
			m_numVertices(numVertices), m_indexCount(indexCount), m_texture(texture), m_tileFactor(tileFactor), 
			m_tintColor(tintColor), m_ignoresCam(ignoresCam)
		{
			m_positionArr = new Vector3[numVertices];
			m_normalArr = new Vector3[numVertices];
			m_indicesArr = new unsigned int[indexCount];
			m_textureCoords = new Vector3[numVertices];

			for (unsigned int i = 0; i < numVertices; i++)
			{
				m_positionArr[i] = vertices[i];
				m_normalArr[i] = normals[i];
				m_textureCoords[i] = textureCoords[i];
			}

			for (unsigned int i = 0; i < indexCount; i++)
			{
				m_indicesArr[i] = indices[i];
			}
		}

		DrawCommand(const DrawCommand& other) : m_transform(other.m_transform), m_mat(other.m_mat),
			m_numVertices(other.m_numVertices), m_indexCount(other.m_indexCount), m_texture(other.m_texture), 
			m_tileFactor(other.m_tileFactor), m_tintColor(other.m_tintColor), m_ignoresCam(other.m_ignoresCam)
		{
			m_positionArr = new Vector3[other.m_numVertices];
			m_normalArr = new Vector3[other.m_numVertices];
			m_indicesArr = new unsigned int[other.m_indexCount];
			m_textureCoords = new Vector3[other.m_numVertices];

			for (unsigned int i = 0; i < other.m_numVertices; i++)
			{
				m_positionArr[i] = other.m_positionArr[i];
				m_normalArr[i] = other.m_normalArr[i];
				m_textureCoords[i] = other.m_textureCoords[i];
			}

			for (unsigned int i = 0; i < other.m_indexCount; i++)
			{
				m_indicesArr[i] = other.m_indicesArr[i];
			}
		}

		~DrawCommand()
		{
			delete[] m_positionArr;
			delete[] m_normalArr;
			delete[] m_indicesArr;
			delete[] m_textureCoords;
		}

		void SendToRenderingBatch()
		{
			constexpr RenderingPrimitive renderingTarget = RenderingPrimitive::Triangles;
			int textureIndex = AddTextureToBatch((*Renderer::s_renderingBatches)[renderingTarget], 
				renderingTarget, m_texture);

			VertexData* vertexData = new VertexData[m_numVertices];

			for (int i = 0; i < m_numVertices; i++)
			{
				vertexData[i].position = (Vector3)(m_transform
					* Vector4(m_positionArr[i].x, m_positionArr[i].y, m_positionArr[i].z, 1));
				vertexData[i].textureCoords = m_textureCoords[i];
				vertexData[i].color = m_tintColor;
				vertexData[i].normal = (Vector3)(m_transform
					* Vector4(m_normalArr[i].x, m_normalArr[i].y, m_normalArr[i].z, 0));
				vertexData[i].textureIndex = textureIndex;
				vertexData[i].tillingFactor = m_tileFactor;
				vertexData[i].uses3DTexture = 0;
				vertexData[i].ignoresCamera = m_ignoresCam ? 1.0f : 0.0f;
				vertexData[i].mat = m_mat;
			}

			(*Renderer::s_renderingBatches)[renderingTarget].Add(vertexData, m_numVertices, m_indicesArr,
				m_indexCount, renderingTarget);

			delete[] vertexData;
		}

		bool operator==(const DrawCommand& other) const
		{
			return  m_mat == other.m_mat 
				&& m_numVertices == other.m_numVertices && m_indexCount == other.m_indexCount
				&& m_tileFactor == other.m_tileFactor && m_tintColor == other.m_tintColor
				&& m_ignoresCam == other.m_ignoresCam && m_transform == other.m_transform;
		}

		bool operator!=(const DrawCommand& other) const
		{
			return !(*this == other);
		}

		DrawCommand& operator=(const DrawCommand& other)
		{
			delete[] m_positionArr;
			delete[] m_normalArr;
			delete[] m_indicesArr;
			delete[] m_textureCoords;

			m_transform = other.m_transform;
			m_mat = other.m_mat;
			m_numVertices = other.m_numVertices;
			m_indexCount = other.m_indexCount;
			m_texture = other.m_texture;
			m_tileFactor = other.m_tileFactor; 
			m_tintColor = other.m_tintColor;
			m_ignoresCam = other.m_ignoresCam;

			m_positionArr = new Vector3[other.m_numVertices];
			m_normalArr = new Vector3[other.m_numVertices];
			m_textureCoords = new Vector3[other.m_numVertices]; 
			m_indicesArr = new unsigned int[other.m_indexCount];

			for (unsigned int i = 0; i < other.m_numVertices; i++)
			{
				m_positionArr[i] = other.m_positionArr[i];
				m_normalArr[i] = other.m_normalArr[i];
				m_textureCoords[i] = other.m_textureCoords[i];
			}

			for (unsigned int i = 0; i < other.m_indexCount; i++)
			{
				m_indicesArr[i] = other.m_indicesArr[i];
			}

			return *this;
		}

	private:
		Mat4 m_transform;
		Material m_mat;
		Vector3* m_positionArr;
		Vector3* m_normalArr;
		unsigned int m_numVertices;
		unsigned int* m_indicesArr;
		unsigned int m_indexCount;
		AReference<Texture> m_texture;
		Vector3* m_textureCoords;
		float m_tileFactor;
		Vector4 m_tintColor;
		bool m_ignoresCam;
	};

	//rendering batch///////////////////////////////////////////////////////////////////

	RenderingBatch::RenderingBatch()
	{
		m_texture2DSlots = new AReference<Texture2D>[s_maxTexture2DSlots];
		m_texture2DShadowMapSlots = new AReference<Texture2D>[s_maxTexture2DShadowMapSlots];
		m_cubemapSlots = new AReference<CubeMap>[s_maxCubemapSlots];
		m_cubemapShadowMapSlots = new AReference<CubeMap>[s_maxCubemapShadowMapSlots];

		m_vertexDataArr = new VertexData[s_maxVertices];
		m_indicesArr = new unsigned int[s_maxIndices];

		m_vbo = VertexBuffer::Create(sizeof(VertexData) * s_maxVertices);

		m_vbo->SetLayout({
			{ ADataType::Float3, "a_position" },
			{ ADataType::Float3, "a_textureCoords" },
			{ ADataType::Float3, "a_normal" },
			{ ADataType::Float4, "a_color" },
			{ ADataType::Float, "a_texIndex" },
			{ ADataType::Float, "a_tillingFactor" },
			{ ADataType::Float, "a_uses3DTexture" },
			{ ADataType::Float, "a_ignoresCamera" },
			{ ADataType::Float, "a_ambiantIntensity" },
			{ ADataType::Float, "a_diffuseIntensity" },
			{ ADataType::Float, "a_specularIntensity" },
			{ ADataType::Float, "a_shininess" },
			{ ADataType::Float, "a_ignoresLighting" }
		});

		m_ibo = IndexBuffer::Create();
	}

	RenderingBatch::RenderingBatch(const RenderingBatch& other) 
		: m_ibo(other.m_ibo), m_vbo(other.m_vbo)
	{
		//no need to copy the data since every frame it gets reset
		m_texture2DSlots = new AReference<Texture2D>[s_maxTexture2DSlots];
		m_texture2DShadowMapSlots = new AReference<Texture2D>[s_maxTexture2DShadowMapSlots];
		m_cubemapSlots = new AReference<CubeMap>[s_maxCubemapSlots];
		m_cubemapShadowMapSlots = new AReference<CubeMap>[s_maxCubemapShadowMapSlots];

		m_vertexDataArr = new VertexData[s_maxVertices];
		m_indicesArr = new unsigned int[s_maxIndices];
	}

	RenderingBatch::~RenderingBatch()
	{
		delete[] m_texture2DSlots;
		delete[] m_texture2DShadowMapSlots;
		delete[] m_cubemapSlots;
		delete[] m_cubemapShadowMapSlots;
		delete[] m_vertexDataArr;
		delete[] m_indicesArr;
	}

	void RenderingBatch::Add(const VertexData* vertices, unsigned int numVertices, const unsigned int* indices,
		unsigned int numIndices, RenderingPrimitive renderTarget)
	{
		if (s_maxVertices <= m_vertexDataIndex + numVertices || s_maxIndices <= m_indicesIndex + numIndices) 
		{
			Draw(renderTarget);
		}

		for (unsigned int i = 0; i < numVertices; i++)
		{
			m_vertexDataArr[m_vertexDataIndex + i] = vertices[i];
		}

		for (unsigned int i = 0; i < numIndices; i++)
		{
			m_indicesArr[m_indicesIndex + i] = m_vertexDataIndex + indices[i];
		}
		m_vertexDataIndex += numVertices;
		m_indicesIndex += numIndices;

		Renderer::s_stats.numIndices += numIndices;
		Renderer::s_stats.numVertices += numVertices;
	}

	//returns texture index for the texture
	int RenderingBatch::AddTexture2D(const AReference<Texture2D>& texture, RenderingPrimitive renderTarget)
	{
		int textureIndex = 0;
		bool found = false;
		for (int i = 0; i < m_texture2DIndex; i++)
		{
			if (*texture == *m_texture2DSlots[i])
			{
				textureIndex = i;
				found = true;
				break;
			}
		}

		if (textureIndex == 0 && !found)
		{
			if (m_texture2DIndex == s_maxTexture2DSlots)
			{
				Draw(renderTarget);
			}

			m_texture2DSlots[m_texture2DIndex] = texture;
			textureIndex = m_texture2DIndex;
			m_texture2DIndex++;
		}

		return textureIndex;
	}

	//returns texture index for the texture
	int RenderingBatch::AddCubemap(AReference<CubeMap> cubemap, RenderingPrimitive renderTarget)
	{
		int textureIndex = 0;
		bool found = false;
		for (int i = 0; i < m_cubemapIndex; i++)
		{
			if (*cubemap == *m_cubemapSlots[i])
			{
				textureIndex = i;
				found = true;
				break;
			}
		}

		if (textureIndex == 0 && !found)
		{
			if (m_cubemapIndex == s_maxCubemapSlots)
			{
				Draw(renderTarget);
			}

			m_cubemapSlots[m_cubemapIndex] = cubemap;
			textureIndex = m_cubemapIndex;
			m_cubemapIndex++;
		}

		return textureIndex;
	}

	void RenderingBatch::AddTexture2DShadowMap(const AReference<Texture2D>& shadowMap)
	{
		int textureIndex = 0;
		bool found = false;
		for (int i = 0; i < m_texture2DShadowMapIndex; i++)
		{
			if (*shadowMap == *m_texture2DShadowMapSlots[i])
			{
				textureIndex = i;
				found = true;
				break;
			}
		}

		if (textureIndex == 0 && !found)
		{
			if (m_texture2DShadowMapIndex == s_maxTexture2DShadowMapSlots)
			{
				//temp (?)
				AE_CORE_ERROR("Insufficient space for additional shadow map for lights");
			}

			m_texture2DShadowMapSlots[m_texture2DShadowMapIndex] = shadowMap;
			m_texture2DShadowMapIndex++;
		}
	}

	void RenderingBatch::AddCubemapShadowMap(const AReference<CubeMap>& shadowMap)
	{
		int textureIndex = 0;
		bool found = false;
		for (int i = 0; i < m_cubemapShadowMapIndex; i++)
		{
			if (*shadowMap == *m_cubemapShadowMapSlots[i])
			{
				textureIndex = i;
				found = true;
				break;
			}
		}

		if (textureIndex == 0 && !found)
		{
			if (m_cubemapShadowMapIndex == s_maxCubemapShadowMapSlots)
			{
				//temp (?)
				AE_CORE_ERROR("Insufficient space for additional cube map for lights");
			}

			m_cubemapShadowMapSlots[m_cubemapShadowMapIndex] = shadowMap;
			m_cubemapShadowMapIndex++;
		}
	}

	//render target is a gl enum ex: GL_TRIANGLES
	void RenderingBatch::Draw(RenderingPrimitive renderTarget)
	{
		m_vbo->SetData(&m_vertexDataArr[0], sizeof(VertexData) * m_vertexDataIndex);

		m_ibo->SetData(&m_indicesArr[0], m_indicesIndex);

		for (unsigned int i = 0; i < m_texture2DIndex; i++)
		{
			m_texture2DSlots[i]->Bind(i);
		}

		unsigned int offset = s_maxTexture2DSlots;

		for (unsigned int i = 0; i < m_cubemapIndex; i++)
		{
			m_cubemapSlots[i]->Bind(i + offset);
		}

		offset += s_maxCubemapSlots;

		for (unsigned int i = 0; i < m_texture2DShadowMapIndex; i++)
		{
			m_texture2DShadowMapSlots[i]->Bind(i + offset);
		}

		offset += s_maxTexture2DShadowMapSlots;

		for (unsigned int i = 0; i < m_cubemapShadowMapIndex; i++)
		{
			m_cubemapShadowMapSlots[i]->Bind(i + offset);
		}

		RenderCommand::DrawIndexed(renderTarget, m_ibo);
		ResetBatch();

		Renderer::s_stats.numDrawCalls++;
	}

	bool RenderingBatch::IsEmpty() const
	{
		return m_indicesIndex == 0 || m_vertexDataIndex == 0;
	}

	void RenderingBatch::ResetBatch()
	{
		m_vertexDataIndex = 0;
		m_indicesIndex = 0;
		m_texture2DIndex = 0;
		m_texture2DShadowMapIndex = 0;
		m_cubemapIndex = 0;
		m_cubemapShadowMapIndex = 0;
	}

	//Renderer///////////////////////////////////////////////////

	void Renderer::Init()
	{
		AE_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();

		s_renderingBatches = new AUnorderedMap<RenderingPrimitive, RenderingBatch>();

		unsigned int whiteTextureData = 0xffffffff;
		s_defaultWhiteCubeMap = CubeMap::Create(1, &whiteTextureData);
		s_defaultWhiteTexture = Texture2D::Create(1, 1, &whiteTextureData, sizeof(whiteTextureData));

		int* samplers = new int[RenderingBatch::s_maxTexture2DSlots];
		for (int i = 0; i < RenderingBatch::s_maxTexture2DSlots; i++)
		{
			samplers[i] = i;
		}

		unsigned int offset = RenderingBatch::s_maxTexture2DSlots;

		int* samplersCubemap = new int[RenderingBatch::s_maxCubemapSlots];
		for (int i = 0; i < RenderingBatch::s_maxCubemapSlots; i++)
		{
			samplersCubemap[i] = i + offset;
		}

		offset += RenderingBatch::s_maxCubemapSlots;

		int* samplers2DShadowMap = new int[RenderingBatch::s_maxTexture2DShadowMapSlots];
		for (int i = 0; i < RenderingBatch::s_maxTexture2DShadowMapSlots; i++)
		{
			samplers2DShadowMap[i] = i + offset;
		}

		offset += RenderingBatch::s_maxTexture2DShadowMapSlots;

		int* samplersCubemapShadowMap = new int[RenderingBatch::s_maxCubemapShadowMapSlots];
		for (int i = 0; i < RenderingBatch::s_maxCubemapShadowMapSlots; i++)
		{
			samplersCubemapShadowMap[i] = i + offset;
		}

		s_shader = Shader::Create("assets/shaders/Shader2D.glsl");
		s_shader->Bind();
		s_shader->SetIntArray("u_texture", samplers, RenderingBatch::s_maxTexture2DSlots);
		s_shader->SetIntArray("u_cubeMap", samplersCubemap, RenderingBatch::s_maxCubemapSlots);
		s_shader->SetIntArray("u_shadow2D", samplers2DShadowMap, RenderingBatch::s_maxTexture2DShadowMapSlots);
		s_shader->SetIntArray("u_cubeMapShadowMap", samplersCubemapShadowMap, RenderingBatch::s_maxCubemapShadowMapSlots);

		delete[] samplers;
		delete[] samplersCubemap;
		delete[] samplers2DShadowMap;
		delete[] samplersCubemapShadowMap;

		/*
		s_directionalLightArr = new DirectionalLight[RenderingBatch::s_maxTexture2DShadowMapSlots];
		s_pointLightArr = new PointLight[RenderingBatch::s_maxCubemapShadowMapSlots];
		*/
	}

	void Renderer::Shutdown()
	{
		delete[] s_directionalLightArr;
		delete[] s_pointLightArr;
		delete s_renderingBatches;
	}

	void Renderer::BeginScene()
	{
		BeginScene(Mat4::Identity(), Vector3::Zero());
	}

	void Renderer::BeginScene(const Mat4& viewProjMatrix, const Vector3& camPos)
	{
		s_shader->Bind();
		s_shader->SetMat4("u_viewProjMatrix", viewProjMatrix);
		s_shader->SetFloat3("u_camPos", camPos);

		s_directionalLightIndex = 0;
		s_pointLightIndex = 0;
	}

	void Renderer::BeginScene(const OrthographicCamera& cam)
	{
		Mat4 viewProjectionMatrix = cam.GetProjectionMatrix() * cam.GetViewMatrix();
		BeginScene(viewProjectionMatrix, cam.GetPosition());
	}

	void Renderer::BeginScene(const RuntimeCamera& cam)
	{
		//view is the identity
		Mat4 viewProjectionMatrix = cam.GetProjectionMatrix();
		BeginScene(viewProjectionMatrix, Vector3::Zero());
	}

	void Renderer::BeginScene(const RuntimeCamera& camera, const TransformComponent& transform)
	{
		Mat4 viewProjectionMatrix = camera.GetProjectionMatrix() * transform.GetTransformMatrix().Inverse();
		BeginScene(viewProjectionMatrix, transform.position);
	}

	void Renderer::EndScene()
	{
		if (s_useShadows && (s_directionalLightIndex != 0 || s_pointLightIndex != 0))
		{
			if (s_updateLights)
			{
				GenerateShadowMaps();
				s_updateLights = false;
			}

			s_shader->Bind();
			s_shader->SetInt("u_useShadows", 1);
			s_shader->SetInt("u_numDirLights", s_directionalLightIndex);
			s_shader->SetInt("u_numPointLights", s_pointLightIndex);
			AddShadowMapToShaders();
			FlushBatch();
		}
		else
		{
			s_shader->Bind();
			s_shader->SetInt("u_useShadows", 0);
			FlushBatch();
		}
		s_drawCommands.Clear();
	}

	//requests that all the light's shadow maps be recalculated for this frame
	void Renderer::UpdateLights()
	{
		s_updateLights = true;
	}

	const AReference<CubeMap>& Renderer::GetDefaultWhiteCubeMap() { return s_defaultWhiteCubeMap; }
	const AReference<Texture2D>& Renderer::GetDefaultWhiteTexture() { return s_defaultWhiteTexture; }

	void Renderer::FlushBatch()
	{
		for (DrawCommand& drawCmd : s_drawCommands) 
		{
			drawCmd.SendToRenderingBatch();
		}

		if (!s_renderingBatches->IsEmpty())
		{
			for (auto& pair : *s_renderingBatches)
			{
				if (!pair.GetElement().IsEmpty())
				{
					pair.GetElement().Draw(pair.GetKey());
				}
			}
		}
	}

	void Renderer::CleanUpAfterShadowMapGeneration()
	{
		AE_CORE_WARN("cleaning up after the shadow map generation is currently turned off");
		/*
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
	}

	void Renderer::AddShadowMapToShaders()
	{
		AE_CORE_WARN("Adding shadow maps to shaders is currently turned off");
		/*
		for (int i = 0; i < s_directionalLightIndex; i++)
		{
			auto shadowMap = s_directionalLightArr[i].GetShadowMap();
			for (auto& pair : s_renderingBatches)
			{
				pair.second.AddTexture2DShadowMap(shadowMap);
			}
		}

		for (int i = 0; i < s_pointLightIndex; i++)
		{
			auto shadowMap = s_pointLightArr[i].GetShadowMap();
			for (auto& pair : s_renderingBatches)
			{
				pair.second.AddCubemapShadowMap(shadowMap);
			}
		}
		*/
	}

	void Renderer::GenerateShadowMaps()
	{
		AE_CORE_WARN("generation of shadow maps is currently turned off");
		/*
		for (unsigned int i = 0; i < s_directionalLightIndex; i++)
		{
			s_directionalLightArr[i].PrepareForShadowMapGeneration();
			FlushBatch();
		}

		for (unsigned int i = 0; i < s_pointLightIndex; i++)
		{
			s_pointLightArr[i].PrepareForShadowMapGeneration();
			//Cannot call FlushBatch directly, because the point light uses a geometry shader which expects only triangles
			//FlushBatch also draws lines, which causes an error with the geometry shader when glDrawElements is eventually called

			s_renderingBatches[GL_TRIANGLES].Draw(GL_TRIANGLES);
		}

		CleanUpAfterShadowMapGeneration();
		*/
	}

	void Renderer::DrawVoxel(const Mat4& transform, const Material& mat, AReference<CubeMap> texture,
		float tileFactor, const Vector4& tintColor)
	{
		UploadVoxel(transform, mat, texture, tileFactor, tintColor);
	}

	void Renderer::DrawVoxel(const Mat4& transform, AReference<CubeMap> texture,
		float tileFactor, const Vector4& tintColor)
	{
		DrawVoxel(transform, s_defaultMaterial, texture, tileFactor, tintColor);
	}

	void Renderer::DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
		const Material& mat, AReference<CubeMap> texture, float tileFactor, const Vector4& tintColor)
	{
		TransformComponent t = TransformComponent(position, rotation, scale);
		DrawVoxel(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor);
	}


	void Renderer::DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
		AReference<CubeMap> texture, float tileFactor, const Vector4& tintColor)
	{
		TransformComponent t = TransformComponent(position, rotation, scale);
		DrawVoxel(t.GetTransformMatrix(), texture, tileFactor, tintColor);
	}

	void Renderer::DrawVoxel(const Mat4& transform, const Material& mat, const Vector4& color)
	{
		DrawVoxel(transform, mat, GetDefaultWhiteCubeMap(), 1, color);
	}

	void Renderer::DrawVoxel(const Mat4& transform, const Vector4& color)
	{
		DrawVoxel(transform, GetDefaultWhiteCubeMap(), 1, color);
	}

	void Renderer::DrawVoxel(const Vector3& position, const Vector3& rotation,
		const Vector3& scale, const Material& mat, const Vector4& color)
	{
		DrawVoxel(position, rotation, scale, mat, GetDefaultWhiteCubeMap(), 1, color);
	}

	void Renderer::DrawVoxel(const Vector3& position, const Vector3& rotation,
		const Vector3& scale, const Vector4& color)
	{
		DrawVoxel(position, rotation, scale, GetDefaultWhiteCubeMap(), 1, color);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		UploadQuad(transform, mat, texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, const AReference<Texture2D>& texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		UploadQuad(transform, s_defaultMaterial, texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
		const Material& mat, const AReference<Texture2D>& texture, float tileFactor,
		const Vector4& tintColor, bool ignoresCam)
	{
		TransformComponent t = TransformComponent(position, rotation, scale);
		DrawQuad(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
		const AReference<Texture2D>& texture, float tileFactor,
		const Vector4& tintColor, bool ignoresCam)
	{
		TransformComponent t = TransformComponent(position, rotation, scale);
		DrawQuad(t.GetTransformMatrix(), texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Material& mat, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(transform, mat, GetDefaultWhiteTexture(), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(transform, GetDefaultWhiteTexture(), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Vector3& rotation,
		const Vector3& scale, const Material& mat, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, rotation, scale, mat, GetDefaultWhiteTexture(), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Vector3& rotation,
		const Vector3& scale, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, rotation, scale, GetDefaultWhiteTexture(), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector2& scale, 
		const AReference<Texture2D>& texture, float tilingFactor, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, Vector3(0, 0, rotation), Vector3(scale.x, scale.y, 1), 
			texture, tilingFactor, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector3& scale,
		const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, Vector3(0, 0, rotation), Vector3(scale.x, scale.y, 1), color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector3& scale, 
		const AReference<Texture2D>& texture, const Vector2* textureCoords, float tilingFactor, 
		const Vector4& tintColor, bool ignoresCam)
	{
		TransformComponent t = TransformComponent(position, Vector3(0, 0, rotation), scale);
		const Vector3 textureCoords3D[] = { 
			Vector3(textureCoords[0].x, textureCoords[0].y, 0.0f),
			Vector3(textureCoords[1].x, textureCoords[1].y, 0.0f),
			Vector3(textureCoords[2].x, textureCoords[2].y, 0.0f),
			Vector3(textureCoords[3].x, textureCoords[3].y, 0.0f)
		};

		UploadQuad(t.GetTransformMatrix(), s_defaultMaterial, texture, textureCoords3D, tilingFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Vector3* vertices,
		unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount, 
		const AReference<Texture2D>& texture, const Vector3* textureCoords, float tileFactor, const Vector4& tintColor)
	{
		UploadVertexData(renderTarget, transform, s_defaultMaterial, vertices, numVertices, normals,
			indices, indexCount, texture, textureCoords, tileFactor, tintColor);
	}

	void Renderer::DrawVertexData(RenderingPrimitive renderTarget, const Vector3& position, const Vector3& rotation,
		const Vector3& scale, const Vector3* vertices, unsigned int numVertices, const Vector3* normals, 
		unsigned int* indices, unsigned int indexCount, const AReference<Texture2D>& texture, const Vector3* textureCoords,
		float tileFactor, const Vector4& tintColor)
	{
		TransformComponent t = TransformComponent(position, rotation, scale);
		DrawVertexData(renderTarget, t.GetTransformMatrix(), vertices, numVertices, normals,
			indices, indexCount, texture, textureCoords, tileFactor, tintColor);
	}

	void Renderer::DrawMesh(const Mat4& transform, AReference<Mesh>& mesh, const AReference<Texture2D>& texture,
		float tileFactor, const Vector4& tintColor)
	{
		UploadMesh(transform, s_defaultMaterial, mesh, texture, tileFactor, tintColor);
	}


	void Renderer::AddDirectionalLight(const Vector3& position, const Vector3& direction, const Vector4& lightColor, bool drawCubes)
	{
		AE_CORE_WARN("Adding of directional lights is currently turned off");
		/*
		AE_CORE_ASSERT(s_directionalLightIndex < RenderingBatch::s_maxTexture2DShadowMapSlots, "too many lights in one frame");

		DirectionalLight currLight(position, direction, lightColor, 512, drawCubes);

		s_shader->Bind();
		s_shader->SetFloat4("u_dirLightColors[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetColor());
		s_shader->SetFloat3("u_dirLightPos[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetPosition());
		s_shader->SetFloat("u_lightRadius[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetRadius());
		s_shader->SetMat4("u_dirLightSpaceMatrices[" + std::to_string(s_directionalLightIndex) + "]",
			currLight.GetLightSpaceMatrix());

		if (s_directionalLightArr[s_directionalLightIndex] != currLight)
		{
			s_updateLights = true;
			s_directionalLightArr[s_directionalLightIndex] = std::move(currLight);
		}

		s_directionalLightIndex++;
		*/
	}

	void Renderer::AddPointLight(const Vector3& position, const Vector4& lightColor, bool drawCubes)
	{
		AE_CORE_WARN("Adding of point lights is currently turned off");
		/*
		assert(s_pointLightIndex < RenderingBatch::s_maxCubemapShadowMapSlots);

		PointLight currLight(position, lightColor, 512, drawCubes);

		s_shader->Bind();
		s_shader->SetFloat4("u_pointLightColors[" + std::to_string(s_pointLightIndex) + "]", currLight.GetColor());
		s_shader->SetFloat3("u_pointLightPos[" + std::to_string(s_pointLightIndex) + "]", currLight.GetPosition());
		s_shader->SetFloat("u_pointLightFarPlanes[" + std::to_string(s_pointLightIndex) + "]", currLight.GetFarPlane());

		if (s_pointLightArr[s_pointLightIndex] != currLight)
		{
			s_updateLights = true;
			s_pointLightArr[s_pointLightIndex] = std::move(currLight);
		}
		s_pointLightIndex++;
		*/
	}

	void Renderer::DrawSprite(const Mat4& transform, const SpriteRendererComponent& sprite, bool ignoresCam)
	{
		DrawQuad(transform, sprite.GetSprite(), 1.0f, sprite.GetColor(), ignoresCam);
	}

	void Renderer::DrawSprite(const Vector3& position, float rotation, const Vector2& size,
		const SpriteRendererComponent& sprite, bool ignoresCam)
	{
		DrawQuad(position, rotation, size, sprite.GetSprite(), 1.0f, sprite.GetColor(), ignoresCam);
	}

	void Renderer::DrawUIElement(const UIElement& element, const Vector4& color)
	{
		Vector3 worldPos = (Vector3)element.GetWorldPos();
		DrawQuad(worldPos, Vector3::Zero(), Vector3(element.GetWorldWidth(),
			element.GetWorldHeight(), 1), color, true);
	}

	const RendererStatistics& Renderer::GetStats()
	{
		return s_stats;
	}

	void Renderer::ResetStats()
	{
		s_stats.Reset();
	}

	void Renderer::UploadVoxel(const Mat4& transform, const Material& mat, AReference<CubeMap> texture,
		float tileFactor, const Vector4& tintColor)
	{
		/*
		constexpr unsigned int renderTarget = 0;
		int textureIndex = (*s_renderingBatches)[renderTarget].AddCubemap(texture, renderTarget);

		Vector3 posAndNormals[] = {
			//back face
			{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },

			//left face
			{ -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },

			//front face
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },

			//right face
			{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },

			//top face
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },

			//bottom face
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }

			/* actual vertices
			{ -0.5f, -0.5f, -0.5f }, //0
			{  0.5f, -0.5f, -0.5f }, //1
			{ -0.5f,  0.5f, -0.5f }, //2
			{  0.5f,  0.5f, -0.5f }, //3
			{ -0.5f, -0.5f,  0.5f }, //4
			{  0.5f, -0.5f,  0.5f }, //5
			{ -0.5f,  0.5f,  0.5f }, //6
			{  0.5f,  0.5f,  0.5f }, //7
			
		};
		*/
		
		Vector3 positions[] = {
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

		Vector3 normals[] = {
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

		/*
		constexpr unsigned int numVertices = sizeof(posAndNormals) / (sizeof(Vector3) * 2);
		VertexData cubeVertices[numVertices];

		unsigned int index = 0;

		for (int i = 0; i < 2 * numVertices; i += 2)
		{
			cubeVertices[index].position = (Vector3)(transform
				* Vector4(posAndNormals[i].x, posAndNormals[i].y, posAndNormals[i].z, 1));
			cubeVertices[index].textureCoords = posAndNormals[i];
			cubeVertices[index].color = tintColor;
			cubeVertices[index].normal = (Vector3)(transform
				* Vector4(posAndNormals[i + 1].x, posAndNormals[i + 1].y, posAndNormals[i + 1].z, 0));
			cubeVertices[index].textureIndex = (float)textureIndex;
			cubeVertices[index].tillingFactor = tileFactor;
			cubeVertices[index].uses3DTexture = 1; //set uses3DTexture to true
			cubeVertices[index].ignoresCamera = 0; //set ignoresCamera to false
			cubeVertices[index].mat = mat;
			index++;
		}
		*/

		unsigned int indices[] = {
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

		s_drawCommands.EmplaceBack(transform, mat, positions, normals, sizeof(positions) / sizeof(Vector3), indices,
			sizeof(indices) / sizeof(unsigned int), texture, positions, tileFactor, tintColor, false);
		/*
		(*s_renderingBatches)[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices,
			sizeof(indices) / sizeof(unsigned int), renderTarget);
		*/
	}

	void Renderer::UploadQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		const Vector3 textureCoords[] = {
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f }
		};

		UploadQuad(transform, mat, texture, textureCoords, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::UploadQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
		const Vector3* textureCoords, float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		Vector3 position[] = {
			{ -0.5f, -0.5f,  0.0f },
			{  0.5f, -0.5f,  0.0f },
			{  0.5f,  0.5f,  0.0f },
			{ -0.5f,  0.5f,  0.0f }
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		Vector3 normals[] = {
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0, 1 }
		};

		s_drawCommands.EmplaceBack(transform, mat, position, normals, sizeof(position) / sizeof(Vector3), indices,
			sizeof(indices) / sizeof(unsigned int), texture, textureCoords, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::UploadVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Material& mat,
		const Vector3* vertices, unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount,
		const AReference<Texture2D>& texture, const Vector3* textureCoords, float tileFactor, const Vector4& tintColor)
	{
		s_drawCommands.EmplaceBack(transform, mat, vertices, normals, numVertices, indices,
			indexCount, texture, textureCoords, tileFactor, tintColor, false);

		/*
		int textureIndex = (*s_renderingBatches)[renderTarget].AddTexture2D(texture, renderTarget);

		VertexData* vertexData = new VertexData[numVertices];

		for (int i = 0; i < numVertices; i++)
		{
			vertexData[i].position = (Vector3)(transform
				* Vector4(vertices[i].x, vertices[i].y, vertices[i].z, 1));
			vertexData[i].textureCoords = textureCoords[i];
			vertexData[i].color = tintColor;
			vertexData[i].normal = vertices[i];
			vertexData[i].textureIndex = textureIndex;
			vertexData[i].tillingFactor = tileFactor;
			vertexData[i].uses3DTexture = 0;
			vertexData[i].ignoresCamera = 0;
			vertexData[i].mat = mat;
		}

		(*s_renderingBatches)[renderTarget].Add(vertexData, numVertices, indices,
			indexCount, renderTarget);

		delete[] vertexData;
		*/
	}

	void Renderer::UploadMesh(const Mat4& transform, const Material& mat, AReference<Mesh>& mesh,
		const AReference<Texture2D>& texture, float tileFactor, const Vector4& tintColor)
	{
		s_drawCommands.EmplaceBack(transform, mat, mesh->GetPositions().GetData(), mesh->GetNormals().GetData(),
			mesh->GetPositions().GetCount(), mesh->GetIndices().GetData(), mesh->GetIndices().GetCount(), texture,
			mesh->GetTextureCoords().GetData(), tileFactor, tintColor, false);
		/*
		constexpr RenderingPrimitive renderTarget = RenderingPrimitive::Triangles;
		int textureIndex = (*s_renderingBatches)[renderTarget].AddTexture2D(texture, renderTarget);

		ADynArr<VertexData> vertexData(mesh->GetPositions().GetCount());

		for (unsigned int i = 0; i < mesh->GetPositions().GetCount(); i++)
		{
			VertexData currVertex;
			currVertex.position = (Vector3)(transform
				* Vector4(mesh->GetPositions()[i].x, mesh->GetPositions()[i].y, mesh->GetPositions()[i].z, 1));
			currVertex.textureCoords 
				= Vector3(mesh->GetTextureCoords()[i].x, mesh->GetTextureCoords()[i].y, 0);
			currVertex.color = tintColor;
			currVertex.normal = (Vector3)(transform
				* Vector4(mesh->GetNormals()[i].x, mesh->GetNormals()[i].y, mesh->GetNormals()[i].z, 0));
			currVertex.textureIndex = textureIndex;
			currVertex.tillingFactor = tileFactor;
			currVertex.uses3DTexture = 0;
			currVertex.ignoresCamera = 0;
			currVertex.mat = mat;
			vertexData.Add(currVertex);
		}

		(*s_renderingBatches)[renderTarget].Add(&vertexData[0], vertexData.GetCount(), &mesh->GetIndices()[0],
			mesh->GetIndices().GetCount(), renderTarget);
		*/
	}

}