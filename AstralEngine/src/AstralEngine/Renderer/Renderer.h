#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Math/AMath.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "OrthographicCamera.h"
#include "AstralEngine/ECS/SceneCamera.h"
#include "AstralEngine/ECS/AEntity.h"
#include "AstralEngine/ECS/Components.h"

namespace AstralEngine
{
	class DirectionalLight;
	class PointLight;
	class Mesh;
	class UIElement;

	class RenderingDataSorter;

	// represents a uniform inside a material
	class MaterialUniform
	{
	public:
		MaterialUniform();
		MaterialUniform(const std::string& name);
		virtual ~MaterialUniform();

		const std::string& GetName() const;
		virtual void SendToShader(AReference<Shader> shader) const = 0;

	protected:
		mutable bool m_hasChanged;

	private:
		std::string m_name;
	};

	class Texture2DUniform : public MaterialUniform
	{
	public:
		Texture2DUniform();
		Texture2DUniform(const std::string& name, Texture2DHandle texture = NullHandle);

		virtual void SendToShader(AReference<Shader> shader) const override;

		void SetTextureSlot(unsigned int textureSlot);
		
		Texture2DHandle GetTexture() const;
		void SetTexture(Texture2DHandle texture);

	private:
		Texture2DHandle m_texture;
		unsigned int m_textureSlot;
	};

	class PrimitiveUniform : public MaterialUniform
	{
	public:
		PrimitiveUniform();
		PrimitiveUniform(const std::string& name, float value);
		PrimitiveUniform(const std::string& name, const Vector2& value);
		PrimitiveUniform(const std::string& name, const Vector3& value);
		PrimitiveUniform(const std::string& name, const Vector4& value);
		PrimitiveUniform(const std::string& name, const Mat3& value);
		PrimitiveUniform(const std::string& name, const Mat4& value);
		PrimitiveUniform(const std::string& name, int value);
		PrimitiveUniform(const std::string& name, const Vector2Int& value);
		PrimitiveUniform(const std::string& name, const Vector3Int& value);
		PrimitiveUniform(const std::string& name, const Vector4Int& value);
		PrimitiveUniform(const std::string& name, bool value);
		~PrimitiveUniform();

		virtual void SendToShader(AReference<Shader> shader) const override;

		ADataType GetType() const;

		template<typename T> 
		const T& GetValue() const
		{
			AE_CORE_ASSERT(m_data != nullptr, "Trying to read invalid uniform");
			return *(T*)m_data;
		}

		void SetValue(float value);
		void SetValue(const Vector2& value);
		void SetValue(const Vector3& value);
		void SetValue(const Vector4& value);
		void SetValue(const Mat3& value);
		void SetValue(const Mat4& value);
		void SetValue(int value);
		void SetValue(const Vector2Int& value);
		void SetValue(const Vector3Int& value);
		void SetValue(const Vector4Int& value);
		void SetValue(bool value);


	private:
		ADataType m_type;
		void* m_data;
	};

	class ArrayUniform : public MaterialUniform
	{
	public:
		ArrayUniform();
		ArrayUniform(const std::string& name, int* arr, unsigned int count);
		virtual ~ArrayUniform();

		virtual void SendToShader(AReference<Shader> shader) const override;

	private:
		void* m_arr;
		unsigned int m_count;
	};

	class Material
	{
	public:
		Material();
		Material(const Vector4& color);
		~Material();

		ShaderHandle GetShader() const;
		void SetShader(ShaderHandle shader);

		Texture2DHandle GetDiffuseMap() const;
		void SetDiffuseMap(Texture2DHandle diffuse);

		Texture2DHandle GetSpecularMap() const;
		void SetSpecularMap(Texture2DHandle specular);

		Texture2DHandle GetTexture(const std::string& name) const;
		bool SetTexture(const std::string& name, Texture2DHandle texture);

		const Vector4& GetColor() const;
		void SetColor(const Vector4& color);

		void AddUniform(MaterialUniform* uniform);
		void RemoveUniform(const std::string& name);

		void SendUniformsToShader();

		static MaterialHandle DefaultMat();
		static MaterialHandle SpriteMat();
		static MaterialHandle MissingMat();

		bool operator==(const Material& other) const;
		bool operator!=(const Material& other) const;

	private:
		static const char* s_diffuseMapName;
		static const char* s_specularMapName;
		static const char* s_colorName;

		MaterialUniform* FindUniformByName(const std::string& name) const;
		Texture2DUniform* FindTextureByName(const std::string& name) const;

		ShaderHandle m_shader;

		ASinglyLinkedList<MaterialUniform*> m_uniforms;
		ASinglyLinkedList<Texture2DUniform*> m_textures;
	};

	/*struch which contains the data to
	  send to the gpu for each vertex

	  the order of the fields is the layout to be sent to the gpu
	*/
	struct VertexData
	{
		Vector3 position;
		Vector2 textureCoords;
		//Vector3 normal;
		//float textureIndex;
		//float tillingFactor;
		//float uses3DTexture;
		//float ignoresCamera;
		//Material mat;

		VertexData() { }
		/*
		VertexData() : textureIndex(-1), tillingFactor(1.0f), 
			uses3DTexture(0.0f), ignoresCamera(0.0f) { }
		*/

		bool operator==(const VertexData& other) const
		{
			return position == other.position && textureCoords == other.textureCoords;
				/*
				&& normal == other.normal
				&& color == other.color
				&& textureIndex == other.textureIndex
				&& tillingFactor == other.tillingFactor
				&& mat == other.mat;
				*/
		}

		bool operator!=(const VertexData& other) const
		{
			return !(*this == other);
		}
	};

	//struct which contains the statistics of the renderer
	struct RendererStatistics
	{
		unsigned int numDrawCalls = 0;
		unsigned int numVertices = 0;
		unsigned int numIndices = 0;
		double timePerFrame; // in seconds

		double GetFrameRate() const
		{
			return 1.0 / timePerFrame;
		}

		void Reset()
		{
			numDrawCalls = 0;
			numVertices = 0;
			numIndices = 0;
		}
	};

	class Renderer
	{
		friend class DrawDataBuffer;
	public:
		static void Init();
		static void Shutdown();

		static const RendererStatistics& GetStats();
		static void ResetStats();

		//use to start renderering and stop rendering
		static void BeginScene(const OrthographicCamera& cam);
		static void BeginScene(const RuntimeCamera& cam);
		static void BeginScene(const RuntimeCamera& camera, const Transform& transform);
		static void EndScene();

		//primitives

		//squares
		static void DrawQuad(const Mat4& transform, MaterialHandle mat, Texture2DHandle texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawQuad(const Mat4& transform, Texture2DHandle texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
			MaterialHandle mat, Texture2DHandle texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
			Texture2DHandle texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawQuad(const Mat4& transform, MaterialHandle mat, 
			const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Mat4& transform, const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
			MaterialHandle mat, const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, const Quaternion& rotation, const Vector3& scale,
			const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, const Vector3& scale,
			const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, float rotation, const Vector2& scale, Texture2DHandle texture,
			float tilingFactor = 1, const Vector4& color = { 1, 1, 1, 1 });

		static void DrawQuad(const Vector3& position, float rotation, const Vector2& scale,
			const Vector4& color = { 1, 1, 1, 1 });

		//sprite
		static void DrawSprite(const Transform& transform, const SpriteRenderer& sprite);

		static void DrawSprite(const Vector3& position, float rotation, const Vector2& size,
			const SpriteRenderer& sprite);

		//mesh
		static void DrawMesh(const Transform& transform, const MeshRenderer& mesh);

		//UI
		static void DrawUIElement(const UIElement& element, const Vector4& color);

	private:
		static RendererStatistics s_stats;
		static RenderingDataSorter s_sorterOpaque;
		static RenderingDataSorter s_sorterTransparent;
		static Mat4 s_viewProjMatrix;
		static double s_frameStartTime;

	};
}