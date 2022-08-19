#include "aepch.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "AstralEngine/ECS/Components.h"
#include "Mesh.h"
#include "AstralEngine/UI/UICore.h"

namespace AstralEngine
{
	// MatUniform ////////////////////////////////////////////////////////////////////////

	MatUniform::MatUniform() : m_data(nullptr) { }
	
	MatUniform::MatUniform(const std::string& name, float value) : m_name(name), m_type(ADataType::Float)
	{
		m_data = new float(value);
	}

	MatUniform::MatUniform(const std::string& name, const Vector2& value) : m_name(name), m_type(ADataType::Float2)
	{
		m_data = new Vector2(value);
	}
	
	MatUniform::MatUniform(const std::string& name, const Vector3& value) : m_name(name), m_type(ADataType::Float3)
	{
		m_data = new Vector3(value);
	}
	
	MatUniform::MatUniform(const std::string& name, const Vector4& value) : m_name(name), m_type(ADataType::Float4)
	{
		m_data = new Vector4(value);
	}
	
	MatUniform::MatUniform(const std::string& name, const Mat3& value) : m_name(name), m_type(ADataType::Mat3)
	{
		m_data = new Mat3(value);
	}

	MatUniform::MatUniform(const std::string& name, const Mat4& value) : m_name(name), m_type(ADataType::Mat4)
	{
		m_data = new Mat4(value);
	}

	MatUniform::MatUniform(const std::string& name, int value) : m_name(name), m_type(ADataType::Int)
	{
		m_data = new int(value);
	}

	MatUniform::MatUniform(const std::string& name, const Vector2Int& value) : m_name(name), m_type(ADataType::Int2)
	{
		m_data = new Vector2Int(value);
	}
	
	MatUniform::MatUniform(const std::string& name, const Vector3Int& value) : m_name(name), m_type(ADataType::Int3)
	{
		m_data = new Vector3Int(value);
	}

	MatUniform::MatUniform(const std::string& name, const Vector4Int& value) : m_name(name), m_type(ADataType::Int4)
	{
		m_data = new Vector4Int(value);
	}

	MatUniform::MatUniform(const std::string& name, bool value) : m_name(name), m_type(ADataType::Bool)
	{
		m_data = new bool(value);
	}
	
	MatUniform::~MatUniform() { delete m_data; }

	void MatUniform::SetToShader(AReference<Shader> shader) const
	{
		if (shader != nullptr)
		{
			AE_CORE_ASSERT(m_data != nullptr, "Trying to set invalid uniform to shader");

			switch(m_type)
			{
			case ADataType::Bool:
				shader->SetBool(m_name, *(bool*)m_data);
				break;

			case ADataType::Float:
				shader->SetFloat(m_name, *(float*)m_data);
				break;

			case ADataType::Float2:
				shader->SetFloat2(m_name, *(Vector2*)m_data);
				break;

			case ADataType::Float3:
				shader->SetFloat3(m_name, *(Vector3*)m_data);
				break;

			case ADataType::Float4:
				shader->SetFloat4(m_name, *(Vector4*)m_data);
				break;

			case ADataType::Int:
				shader->SetInt(m_name, *(int*)m_data);
				break;

			case ADataType::Int2:
				shader->SetInt2(m_name, *(Vector2Int*)m_data);
				break;

			case ADataType::Int3:
				shader->SetInt3(m_name, *(Vector3Int*)m_data);
				break;

			case ADataType::Int4:
				shader->SetInt4(m_name, *(Vector4Int*)m_data);
				break;

			case ADataType::Mat3:
				shader->SetMat3(m_name, *(Mat3*)m_data);
				break;

			case ADataType::Mat4:
				shader->SetMat4(m_name, *(Mat4*)m_data);
				break;
			}
		}
	}

	// Material //////////////////////////////////////////////////////////////////////////

	Material::Material() : m_diffuseMap(Texture2D::WhiteTexture()), 
		m_specularMap(Texture2D::WhiteTexture()), m_color(1.0f, 1.0f, 1.0f, 1.0f) { }

	Material::Material(const Vector4& color) : m_diffuseMap(Texture2D::WhiteTexture()),
		m_specularMap(Texture2D::WhiteTexture()), m_color(color) { }

	ShaderHandle Material::GetShader() const { return m_shader; }
	void Material::SetShader(ShaderHandle shader) { m_shader = shader; }

	Texture2DHandle Material::GetDiffuseMap() const { return m_diffuseMap; }
	void Material::SetDiffuseMap(Texture2DHandle diffuse) { m_diffuseMap = diffuse; }

	Texture2DHandle Material::GetSpecularMap() const { return m_specularMap; }
	void Material::SetSpecularMap(Texture2DHandle specular) { m_specularMap = specular; }

	const Vector4& Material::GetColor() const { return m_color; }
	Vector4& Material::GetColor() { return m_color; }
	void Material::SetColor(const Vector4& color) { m_color = color; }

	MaterialHandle Material::DefaultMat()
	{
		static MaterialHandle defaultMat = ResourceHandler::CreateMaterial();
		return defaultMat;
	}

	MaterialHandle Material::MissingMat()
	{
		static MaterialHandle missingMat = ResourceHandler::CreateMaterial(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		return missingMat;
	}

	bool Material::operator==(const Material& other) const
	{
		return m_shader == other.m_shader && m_diffuseMap == other.m_diffuseMap
			&& m_specularMap == other.m_specularMap;
	}

	bool Material::operator!=(const Material& other) const
	{
		return !(*this == other);
	}


	////////////////////////////////////////////////////////////////////////////////////////

	AUnorderedMap<RenderingPrimitive, RenderingBatch>* Renderer::s_renderingBatches;
	RendererStatistics Renderer::s_stats;

	unsigned int RenderingBatch::s_maxVertices = 64000;
	unsigned int RenderingBatch::s_maxIndices = 72000;
	unsigned int RenderingBatch::s_maxTextureSlots = 32;
	unsigned int RenderingBatch::s_maxTexture2DShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxTexture2DSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxCubemapSlots = RenderingBatch::s_maxTextureSlots / 4;
	unsigned int RenderingBatch::s_maxCubemapShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;


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
	int RenderingBatch::AddTexture2D(AReference<Texture2D> texture, RenderingPrimitive renderTarget)
	{
		int textureIndex = 0;
		bool found = false;
		for (unsigned int i = 0; i < m_texture2DIndex; i++)
		{
			if (*texture == *m_texture2DSlots[i])
			{
				textureIndex = (int)i;
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
		for (unsigned int i = 0; i < m_cubemapIndex; i++)
		{
			if (*cubemap == *m_cubemapSlots[i])
			{
				textureIndex = (int)i;
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

	void RenderingBatch::AddTexture2DShadowMap(AReference<Texture2D> shadowMap)
	{
		int textureIndex = -1;
		for (unsigned int i = 0; i < m_texture2DShadowMapIndex; i++)
		{
			if (*shadowMap == *m_texture2DShadowMapSlots[i])
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == -1)
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

	void RenderingBatch::AddCubemapShadowMap(AReference<CubeMap> shadowMap)
	{
		int textureIndex = -1;
		for (unsigned int i = 0; i < m_cubemapShadowMapIndex; i++)
		{
			if (*shadowMap == *m_cubemapShadowMapSlots[i])
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == -1)
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

		int* samplers = new int[RenderingBatch::s_maxTexture2DSlots];
		for (int i = 0; i < (int)RenderingBatch::s_maxTexture2DSlots; i++)
		{
			samplers[i] = (int)i;
		}

		unsigned int offset = RenderingBatch::s_maxTexture2DSlots;

		int* samplersCubemap = new int[RenderingBatch::s_maxCubemapSlots];
		for (int i = 0; i < (int)RenderingBatch::s_maxCubemapSlots; i++)
		{
			samplersCubemap[i] = i + offset;
		}

		offset += RenderingBatch::s_maxCubemapSlots;

		int* samplers2DShadowMap = new int[RenderingBatch::s_maxTexture2DShadowMapSlots];
		for (unsigned int i = 0; i < RenderingBatch::s_maxTexture2DShadowMapSlots; i++)
		{
			samplers2DShadowMap[i] = (int)(i + offset);
		}

		offset += RenderingBatch::s_maxTexture2DShadowMapSlots;

		int* samplersCubemapShadowMap = new int[RenderingBatch::s_maxCubemapShadowMapSlots];
		for (unsigned int i = 0; i < RenderingBatch::s_maxCubemapShadowMapSlots; i++)
		{
			samplersCubemapShadowMap[i] = (int)(i + offset);
		}

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
		delete s_renderingBatches;
	}

	void Renderer::BeginScene(const OrthographicCamera& cam)
	{
		Mat4 viewProjectionMatrix = cam.GetProjectionMatrix() * cam.GetViewMatrix();

	}

	void Renderer::BeginScene(const RuntimeCamera& cam)
	{
		//view is the identity
		Mat4 viewProjectionMatrix = cam.GetProjectionMatrix();
	}

	void Renderer::BeginScene(const RuntimeCamera& camera, const Transform& transform)
	{
		Mat4 viewProjectionMatrix = camera.GetProjectionMatrix() * transform.GetTransformMatrix().Inverse();
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::FlushBatch()
	{
		for (auto& pair : *s_renderingBatches)
		{
			if (!pair.GetElement().IsEmpty())
			{
				pair.GetElement().Draw(pair.GetKey());
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

	
	void Renderer::DrawQuad(const Mat4& transform, const Material& mat, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		UploadQuad(transform, mat, texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		UploadQuad(transform, *ResourceHandler::GetMaterial(Material::DefaultMat()).Get(), 
			texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
		const Material& mat, AReference<Texture2D> texture, float tileFactor,
		const Vector4& tintColor, bool ignoresCam)
	{
		Transform t = Transform(position, rotation, scale);
		DrawQuad(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
		AReference<Texture2D> texture, float tileFactor,
		const Vector4& tintColor, bool ignoresCam)
	{
		Transform t = Transform(position, rotation, scale);
		DrawQuad(t.GetTransformMatrix(), texture, tileFactor, tintColor, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Material& mat, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(transform, mat, ResourceHandler::GetTexture2D(Texture2D::WhiteTexture()), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(transform, ResourceHandler::GetTexture2D(Texture2D::WhiteTexture()), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation,
		const Vector3& scale, const Material& mat, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, rotation, scale, mat, ResourceHandler::GetTexture2D(Texture2D::WhiteTexture()), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation,
		const Vector3& scale, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, rotation, scale, ResourceHandler::GetTexture2D(Texture2D::WhiteTexture()), 1.0f, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector2& scale, AReference<Texture2D>& texture,
		float tilingFactor, const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, Quaternion::EulerToQuaternion(0, 0, rotation), Vector3(scale.x, scale.y, 1), 
			texture, tilingFactor, color, ignoresCam);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector2& scale,
		const Vector4& color, bool ignoresCam)
	{
		DrawQuad(position, Quaternion::EulerToQuaternion(0, 0, rotation), Vector3(scale.x, scale.y, 1), color, ignoresCam);
	}

	void Renderer::DrawVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Vector3* vertices,
		unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount, AReference<Texture2D> texture,
		const Vector3* textureCoords, float tileFactor, const Vector4& tintColor)
	{
		UploadVertexData(renderTarget, transform, *ResourceHandler::GetMaterial(Material::DefaultMat()).Get(), 
			vertices, numVertices, normals, indices, indexCount, texture, textureCoords, tileFactor, tintColor);
	}

	void Renderer::DrawVertexData(RenderingPrimitive renderTarget, const Vector3& position, const Quaternion& rotation,
		const Vector3& scale, const Vector3* vertices, unsigned int numVertices, const Vector3* normals, 
		unsigned int* indices, unsigned int indexCount, AReference<Texture2D> texture, const Vector3* textureCoords,
		float tileFactor, const Vector4& tintColor)
	{
		Transform t = Transform(position, rotation, scale);
		DrawVertexData(renderTarget, t.GetTransformMatrix(), vertices, numVertices, normals,
			indices, indexCount, texture, textureCoords, tileFactor, tintColor);
	}

	void Renderer::DrawMesh(const Mat4& transform, AReference<Mesh>& mesh, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor)
	{
		UploadMesh(transform, *ResourceHandler::GetMaterial(Material::DefaultMat()).Get(), 
			mesh, texture, tileFactor, tintColor);
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

	void Renderer::DrawSprite(const Mat4& transform, const SpriteRenderer& sprite, bool ignoresCam)
	{
		DrawQuad(transform, sprite.GetSprite(), 1.0f, sprite.GetColor(), ignoresCam);
	}

	void Renderer::DrawSprite(const Vector3& position, float rotation, const Vector2& size,
		const SpriteRenderer& sprite, bool ignoresCam)
	{
		DrawQuad(position, rotation, size, sprite.GetSprite(), 1.0f, sprite.GetColor(), ignoresCam);
	}

	void Renderer::DrawUIElement(const UIElement& element, const Vector4& color)
	{
		Vector3 worldPos = (Vector3)element.GetWorldPos();
		DrawQuad(worldPos, Quaternion::Identity(), Vector3(element.GetWorldWidth(),
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

		/*
		(*s_renderingBatches)[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices,
			sizeof(indices) / sizeof(unsigned int), renderTarget);
		*/
	}

	void Renderer::UploadQuad(const Mat4& transform, const Material& mat, AReference<Texture2D> texture,
		float tileFactor, const Vector4& tintColor, bool ignoresCam)
	{
		const Vector3 textureCoords[] = {
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f }
		};

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

	}

	void Renderer::UploadVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Material& mat,
		const Vector3* vertices, unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount,
		AReference<Texture2D> texture, const Vector3* textureCoords, float tileFactor, const Vector4& tintColor)
	{
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
		AReference<Texture2D> texture, float tileFactor, const Vector4& tintColor)
	{
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