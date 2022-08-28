#include "aepch.h"
#include "Renderer.h"
#include "AstralEngine/ECS/Components.h"
#include "Mesh.h"
#include "AstralEngine/UI/UICore.h"
#include "AstralEngine/Renderer/RendererInternals.h"
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	// MaterialUniform /////////////////////////////////
	MaterialUniform::MaterialUniform() : m_hasChanged(true) { }
	MaterialUniform::MaterialUniform(const std::string& name) : m_name(name), m_hasChanged(true) { }
	MaterialUniform::~MaterialUniform() { }

	const std::string& MaterialUniform::GetName() const { return m_name; }

	// Texture2DUniform //////////////////////////////////////////////////////////////////////////

	Texture2DUniform::Texture2DUniform() { }
	Texture2DUniform::Texture2DUniform(const std::string& name, Texture2DHandle texture) 
		: MaterialUniform(name), m_texture(texture) { }

	void Texture2DUniform::SendToShader(AReference<Shader> shader) const
	{
		if (!m_hasChanged)
		{
			return;
		}

		AE_RENDER_ASSERT(m_texture != NullHandle, "Trying to send invalid uniform to shader");
		if (shader != nullptr)
		{
			AReference<Texture2D>& texture = ResourceHandler::GetTexture2D(m_texture);
			if (texture != nullptr)
			{
				shader->SetInt(GetName(), m_textureSlot);
				texture->Bind(m_textureSlot);
				m_hasChanged = true;
			}
		}
	}

	void Texture2DUniform::SetTextureSlot(unsigned int textureSlot) 
	{ 
		m_textureSlot = textureSlot; 
		m_hasChanged = true;
	}
	
	Texture2DHandle Texture2DUniform::GetTexture() const { return m_texture; }
	void Texture2DUniform::SetTexture(Texture2DHandle texture) 
	{ 
		m_texture = texture; 
		m_hasChanged = true;
	}

	// PrimitiveUniform ////////////////////////////////////////////////////////////////////////

	PrimitiveUniform::PrimitiveUniform() : m_data(nullptr) { }
	
	PrimitiveUniform::PrimitiveUniform(const std::string& name, float value) : MaterialUniform(name), 
		m_type(ADataType::Float)
	{
		m_data = new float(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector2& value) : MaterialUniform(name),
		m_type(ADataType::Float2)
	{
		m_data = new Vector2(value);
	}
	
	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector3& value) : MaterialUniform(name),
		m_type(ADataType::Float3)
	{
		m_data = new Vector3(value);
	}
	
	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector4& value) : MaterialUniform(name),
		m_type(ADataType::Float4)
	{
		m_data = new Vector4(value);
	}
	
	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Mat3& value) : MaterialUniform(name),
		m_type(ADataType::Mat3)
	{
		m_data = new Mat3(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Mat4& value) : MaterialUniform(name),
		m_type(ADataType::Mat4)
	{
		m_data = new Mat4(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, int value) : MaterialUniform(name),
		m_type(ADataType::Int)
	{
		m_data = new int(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector2Int& value) : MaterialUniform(name),
		m_type(ADataType::Int2)
	{
		m_data = new Vector2Int(value);
	}
	
	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector3Int& value) : MaterialUniform(name),
		m_type(ADataType::Int3)
	{
		m_data = new Vector3Int(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, const Vector4Int& value) : MaterialUniform(name),
		m_type(ADataType::Int4)
	{
		m_data = new Vector4Int(value);
	}

	PrimitiveUniform::PrimitiveUniform(const std::string& name, bool value) : MaterialUniform(name),
		m_type(ADataType::Bool)
	{
		m_data = new bool(value);
	}

	PrimitiveUniform::~PrimitiveUniform() { delete m_data; }

	void PrimitiveUniform::SendToShader(AReference<Shader> shader) const
	{
		if (!m_hasChanged)
		{
			return;
		}

		AE_RENDER_ASSERT(m_data != nullptr, "Trying to send invalid uniform to shader");
		if (shader != nullptr)
		{
			switch (m_type)
			{
			case ADataType::Bool:
				shader->SetBool(GetName(), *(bool*)m_data);
				break;

			case ADataType::Float:
				shader->SetFloat(GetName(), *(float*)m_data);
				break;

			case ADataType::Float2:
				shader->SetFloat2(GetName(), *(Vector2*)m_data);
				break;

			case ADataType::Float3:
				shader->SetFloat3(GetName(), *(Vector3*)m_data);
				break;

			case ADataType::Float4:
				shader->SetFloat4(GetName(), *(Vector4*)m_data);
				break;

			case ADataType::Int:
				shader->SetInt(GetName(), *(int*)m_data);
				break;

			case ADataType::Int2:
				shader->SetInt2(GetName(), *(Vector2Int*)m_data);
				break;

			case ADataType::Int3:
				shader->SetInt3(GetName(), *(Vector3Int*)m_data);
				break;

			case ADataType::Int4:
				shader->SetInt4(GetName(), *(Vector4Int*)m_data);
				break;

			case ADataType::Mat3:
				shader->SetMat3(GetName(), *(Mat3*)m_data);
				break;

			case ADataType::Mat4:
				shader->SetMat4(GetName(), *(Mat4*)m_data);
				break;
			}
			m_hasChanged = false;
		}
	}
	
	ADataType PrimitiveUniform::GetType() const { return m_type; }

	void PrimitiveUniform::SetValue(float value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Float, "Assigning incorrect type to uniform %S", GetName());
		float* data = (float*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector2& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Float2, "Assigning incorrect type to uniform %S", GetName());
		Vector2* data = (Vector2*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector3& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Float3, "Assigning incorrect type to uniform %S", GetName());
		Vector3* data = (Vector3*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector4& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Float4, "Assigning incorrect type to uniform %S", GetName());
		Vector4* data = (Vector4*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Mat3& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Mat3, "Assigning incorrect type to uniform %S", GetName());
		Mat3* data = (Mat3*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Mat4& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Mat4, "Assigning incorrect type to uniform %S", GetName());
		Mat4* data = (Mat4*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(int value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Int, "Assigning incorrect type to uniform %S", GetName());
		int* data = (int*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector2Int& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Int2, "Assigning incorrect type to uniform %S", GetName());
		Vector2Int* data = (Vector2Int*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector3Int& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Int3, "Assigning incorrect type to uniform %S", GetName());
		Vector3Int* data = (Vector3Int*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(const Vector4Int& value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Int3, "Assigning incorrect type to uniform %S", GetName());
		Vector3Int* data = (Vector3Int*)m_data;
		*data = value;
		m_hasChanged = true;
	}

	void PrimitiveUniform::SetValue(bool value)
	{
		AE_RENDER_ASSERT(m_type == ADataType::Bool, "Assigning incorrect type to uniform %S", GetName());
		bool* data = (bool*)m_data;
		*data = value;
		m_hasChanged = true;
	}


	// ArrayUniform //////////////////////////////////////////////////////////////////
	ArrayUniform::ArrayUniform() : m_arr(nullptr), m_count(0) { }
	ArrayUniform::ArrayUniform(const std::string& name, int* arr, unsigned int count)
		: MaterialUniform(name), m_arr(arr), m_count(count) { }
	
	ArrayUniform::~ArrayUniform() 
	{
		delete[] m_arr;
	}

	void ArrayUniform::SendToShader(AReference<Shader> shader) const
	{
		if (!m_hasChanged)
		{
			return;
		}

		AE_RENDER_ASSERT(m_arr != nullptr, "Trying to send invalid uniform to shader");
		if (shader != nullptr)
		{
			shader->SetIntArray(GetName(), (int*)m_arr, m_count);
			m_hasChanged = false;
		}
	}



	// Material //////////////////////////////////////////////////////////////////////////

	const char* Material::s_diffuseMapName = "u_diffuseMap";
	const char* Material::s_specularMapName = "u_specularMap";
	const char* Material::s_colorName = "u_matColor";

	Material::Material() : m_shader(Shader::DefaultShader()) { }

	Material::Material(const Vector4& color) : m_shader(Shader::DefaultShader()) 
	{
		SetColor(color);
	}

	Material::~Material()
	{
		for (MaterialUniform* uniform : m_uniforms)
		{
			delete uniform;
		}
	}

	ShaderHandle Material::GetShader() const { return m_shader; }
	
	void Material::SetShader(ShaderHandle shader)
	{
		if (shader == NullHandle)
		{
			m_shader = Shader::DefaultShader();
		}
		else
		{
			m_shader = shader;
		}
	}

	Texture2DHandle Material::GetDiffuseMap() const 
	{ 
		Texture2DUniform* diffuse = FindTextureByName(s_diffuseMapName);
		if (diffuse == nullptr)
		{
			return NullHandle;
		}
		return diffuse->GetTexture();
	}

	void Material::SetDiffuseMap(Texture2DHandle diffuse) 
	{ 
		Texture2DUniform* diffuseUniform = FindTextureByName(s_diffuseMapName);
		if (diffuseUniform == nullptr)
		{
			AddUniform(new Texture2DUniform(s_diffuseMapName, diffuse));
		}
		else
		{
			diffuseUniform->SetTexture(diffuse);
		}
	}

	Texture2DHandle Material::GetSpecularMap() const 
	{ 
		AE_CORE_ERROR("not implemented yet");
		return NullHandle; 
	}
	void Material::SetSpecularMap(Texture2DHandle specular) 
	{ 
		AE_CORE_ERROR("not implemented yet");
	}

	Texture2DHandle Material::GetTexture(const std::string& name) const
	{
		Texture2DUniform* texture = FindTextureByName(name);
		if (texture == nullptr)
		{
			return NullHandle;
		}
		return texture->GetTexture();
	}

	bool Material::SetTexture(const std::string& name, Texture2DHandle texture)
	{
		Texture2DUniform* texturePtr = FindTextureByName(name);
		if (texturePtr == nullptr)
		{
			return false;
		}

		texturePtr->SetTexture(texture);
	}

	const Vector4& Material::GetColor() const 
	{ 
		MaterialUniform* uniform = FindUniformByName(s_colorName);
		if (uniform == nullptr)
		{
			return Vector4::Zero();
		}
		PrimitiveUniform* primitive = dynamic_cast<PrimitiveUniform*>(uniform);
		return primitive->GetValue<Vector4>();
	}
	
	void Material::SetColor(const Vector4& color) 
	{ 
		MaterialUniform* uniform = FindUniformByName(s_colorName);
		if (uniform == nullptr)
		{
			uniform = new PrimitiveUniform(s_colorName, color);
			AddUniform(uniform);
		}
		else
		{
			PrimitiveUniform* primitive = dynamic_cast<PrimitiveUniform*>(uniform);
			AE_RENDER_ASSERT(primitive != nullptr, "");
			primitive->SetValue(color);
		}
	}

	void Material::AddUniform(MaterialUniform* uniform)
	{
		m_uniforms.Add(uniform);
		Texture2DUniform* texture = dynamic_cast<Texture2DUniform*>(uniform);
		if (texture != nullptr)
		{
			m_textures.Add(texture);
		}
	}

	void Material::RemoveUniform(const std::string& name)
	{
		for (auto it = m_uniforms.begin(); it != m_uniforms.end(); it++)
		{
			if ((*it)->GetName() == name)
			{
				if (m_textures.Contains((Texture2DUniform*)*it))
				{
					m_textures.Remove((Texture2DUniform*)*it);
				}
				m_uniforms.Remove(it);
				break;
			}
		}
	}

	void Material::SendUniformsToShader()
	{
		AReference<Shader>& shader = ResourceHandler::GetShader(m_shader);

		if (shader != nullptr)
		{
			unsigned int textureSlot = 0;
			for (Texture2DUniform* texture : m_textures)
			{
				texture->SetTextureSlot(textureSlot);
				texture->SendToShader(shader);
				textureSlot++;
			}

			for (MaterialUniform* uniform : m_uniforms)
			{
				if (!m_textures.Contains((Texture2DUniform*)uniform))
				{
					uniform->SendToShader(shader);
				}
			}
		}
	}

	MaterialHandle Material::DefaultMat()
	{
		static MaterialHandle defaultMat = NullHandle;
		if (defaultMat == NullHandle)
		{
			defaultMat = ResourceHandler::CreateMaterial({ 1.0f, 1.0f, 1.0f, 1.0f });
			AReference<Material> material = ResourceHandler::GetMaterial(defaultMat);
			material->SetDiffuseMap(Texture2D::WhiteTexture());
		}
		return defaultMat;
	}

	MaterialHandle Material::SpriteMat()
	{
		static MaterialHandle spriteMat = NullHandle;
		if (spriteMat == NullHandle)
		{
			spriteMat = ResourceHandler::CreateMaterial();
			AReference<Material>& sprite = ResourceHandler::GetMaterial(spriteMat);
			sprite->SetShader(Shader::SpriteShader());
			size_t numTextureSlots = RenderCommand::GetNumTextureSlots();
			int* textureSlots = new int[numTextureSlots];

			for (size_t i = 0; i < numTextureSlots; i++)
			{
				textureSlots[i] = i;
			}

			sprite->AddUniform(new ArrayUniform("u_textures", textureSlots, (unsigned int)numTextureSlots));
		}
		return spriteMat;
	}

	MaterialHandle Material::MissingMat()
	{
		static MaterialHandle missingMat = ResourceHandler::CreateMaterial(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		return missingMat;
	}

	bool Material::operator==(const Material& other) const
	{
		return m_shader == other.m_shader && m_uniforms == other.m_uniforms;
	}

	bool Material::operator!=(const Material& other) const
	{
		return !(*this == other);
	}

	MaterialUniform* Material::FindUniformByName(const std::string& name) const
	{
		for(MaterialUniform* uniform : m_uniforms)
		{
			if (uniform->GetName() == name)
			{
				return uniform;
			}
		}
		return nullptr;
	}

	Texture2DUniform* Material::FindTextureByName(const std::string& name) const
	{
		MaterialUniform* uniform = FindUniformByName(name);
		if (uniform == nullptr || !m_textures.Contains((Texture2DUniform*)uniform))
		{
			return nullptr;
		}
		return dynamic_cast<Texture2DUniform*>(uniform);
	}


	//Renderer///////////////////////////////////////////////////

	RenderingDataSorter Renderer::s_sorterOpaque;
	RenderingDataSorter Renderer::s_sorterTransparent;
	RendererStatistics Renderer::s_stats;
	Mat4 Renderer::s_viewProjMatrix;
	double Renderer::s_frameStartTime;

	void Renderer::Init()
	{
		
		RenderCommand::Init();
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginScene(const OrthographicCamera& cam)
	{
		s_frameStartTime = Time::GetTime();
		s_viewProjMatrix = cam.GetProjectionMatrix() * cam.GetViewMatrix();
		s_sorterOpaque.Clear();
		s_sorterTransparent.Clear();
	}

	void Renderer::BeginScene(const RuntimeCamera& cam)
	{
		s_frameStartTime = Time::GetTime();
		//view is the identity
		s_viewProjMatrix = cam.GetProjectionMatrix();
		s_sorterOpaque.Clear();
		s_sorterTransparent.Clear();
	}

	void Renderer::BeginScene(const RuntimeCamera& camera, const Transform& transform)
	{
		s_frameStartTime = Time::GetTime();
		s_viewProjMatrix = camera.GetProjectionMatrix() * transform.GetTransformMatrix().Inverse();
		s_sorterOpaque.Clear();
		s_sorterTransparent.Clear();
	}

	void Renderer::EndScene()
	{		
		

		for (auto& pair : s_sorterOpaque)
		{
			pair.GetElement().Draw(s_viewProjMatrix, pair.GetKey());
		}

		for (auto& pair : s_sorterTransparent)
		{
			pair.GetElement().Draw(s_viewProjMatrix, pair.GetKey());
		}
		
		s_stats.timePerFrame = Time::GetTime() - s_frameStartTime;
	}
	
	void Renderer::DrawQuad(const Mat4& transform, MaterialHandle mat, Texture2DHandle texture,
		float tileFactor, const Vector4& tintColor)
	{
		DrawCommand* cmd = new DrawCommand(transform, mat, Mesh::QuadMesh(), tintColor, NullEntity, 
			(tintColor.a == 1.0f));
		if (cmd->IsOpaque())
		{
			s_sorterOpaque.AddData(cmd);
		}
		else
		{
			s_sorterTransparent.AddData(cmd);
		}
	}

	void Renderer::DrawQuad(const Mat4& transform, Texture2DHandle texture,
		float tileFactor, const Vector4& tintColor)
	{
		DrawQuad(transform, Material::DefaultMat(), texture, tileFactor, tintColor);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
		MaterialHandle mat, Texture2DHandle texture, float tileFactor,
		const Vector4& tintColor)
	{
		Transform t = Transform(position, rotation, scale);
		DrawQuad(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
		Texture2DHandle texture, float tileFactor, const Vector4& tintColor)
	{
		DrawQuad(position, rotation, scale, Material::DefaultMat(), texture, tileFactor, tintColor);
	}

	void Renderer::DrawQuad(const Mat4& transform, MaterialHandle mat, const Vector4& color)
	{
		DrawQuad(transform, mat, Texture2D::WhiteTexture(), 1.0f, color);
	}

	void Renderer::DrawQuad(const Mat4& transform, const Vector4& color)
	{
		DrawQuad(transform, Texture2D::WhiteTexture(), 1.0f, color);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation,
		const Vector3& scale, MaterialHandle mat, const Vector4& color)
	{
		DrawQuad(position, rotation, scale, mat, Texture2D::WhiteTexture(), 1.0f, color);
	}

	void Renderer::DrawQuad(const Vector3& position, const Quaternion& rotation,
		const Vector3& scale, const Vector4& color)
	{
		DrawQuad(position, rotation, scale, Texture2D::WhiteTexture(), 1.0f, color);
	}

	void Renderer::DrawQuad(const Vector3& position, const Vector3& scale, const Vector4& color)
	{
		DrawQuad(position, Quaternion::Identity(), scale, color);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector2& scale, Texture2DHandle texture,
		float tilingFactor, const Vector4& color)
	{
		DrawQuad(position, Quaternion::EulerToQuaternion(0, 0, rotation), Vector3(scale.x, scale.y, 1), 
			texture, tilingFactor, color);
	}

	void Renderer::DrawQuad(const Vector3& position, float rotation, const Vector2& scale,
		const Vector4& color)
	{
		DrawQuad(position, Quaternion::EulerToQuaternion(0, 0, rotation), Vector3(scale.x, scale.y, 1), color);
	}

	void Renderer::DrawSprite(const Transform& transform, const SpriteRenderer& sprite)
	{
		DrawCommand* cmd = new DrawCommand(transform.GetTransformMatrix(), Material::SpriteMat(),
			Mesh::QuadMesh(), sprite.GetColor(), transform.GetAEntity(), sprite.GetColor().a == 1.0f, sprite.GetSprite());

		if (cmd->IsOpaque())
		{
			s_sorterOpaque.AddData(cmd);
		}
		else
		{
			s_sorterTransparent.AddData(cmd);
		}
	}

	void Renderer::DrawSprite(const Vector3& position, float rotation, const Vector2& size,
		const SpriteRenderer& sprite)
	{
		DrawQuad(position, rotation, size, sprite.GetSprite(), 1.0f, sprite.GetColor());
	}


	void Renderer::DrawMesh(const Transform& transform, const MeshRenderer& mesh)
	{
		if (mesh.GetMesh() != NullHandle)
		{
			AReference<Material> m = ResourceHandler::GetMaterial(mesh.GetMaterial());
			DrawCommand* cmd = new DrawCommand(transform.GetTransformMatrix(), mesh.GetMaterial(),
				mesh.GetMesh(), Vector4(1.0f, 1.0f, 1.0f, 1.0f), transform.GetAEntity(), (m->GetColor().a == 1.0f));

			if (cmd->IsOpaque())
			{
				s_sorterOpaque.AddData(cmd);
			}
			else
			{
				s_sorterTransparent.AddData(cmd);
			}
		}
	}

	void Renderer::DrawUIElement(const UIElement& element, const Vector4& color)
	{
		Vector3 worldPos = (Vector3)element.GetWorldPos();
		DrawQuad(worldPos, Quaternion::Identity(), Vector3(element.GetWorldWidth(),
			element.GetWorldHeight(), 1), color);
	}

	const RendererStatistics& Renderer::GetStats()
	{
		return s_stats;
	}

	void Renderer::ResetStats()
	{
		s_stats.Reset();
	}
}