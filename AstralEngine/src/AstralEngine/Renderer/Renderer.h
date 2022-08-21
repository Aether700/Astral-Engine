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
	class MatUniform
	{
	public:
		MatUniform();
		MatUniform(const std::string& name, float value);
		MatUniform(const std::string& name, const Vector2& value);
		MatUniform(const std::string& name, const Vector3& value);
		MatUniform(const std::string& name, const Vector4& value);
		MatUniform(const std::string& name, const Mat3& value);
		MatUniform(const std::string& name, const Mat4& value);
		MatUniform(const std::string& name, int value);
		MatUniform(const std::string& name, const Vector2Int& value);
		MatUniform(const std::string& name, const Vector3Int& value);
		MatUniform(const std::string& name, const Vector4Int& value);
		MatUniform(const std::string& name, bool value);
		~MatUniform();

		void SetToShader(AReference<Shader> shader) const;

	private:
		std::string m_name;
		ADataType m_type;
		void* m_data;
	};

	class Material
	{
	public:
		Material();
		Material(const Vector4& color);

		ShaderHandle GetShader() const;
		void SetShader(ShaderHandle shader);

		Texture2DHandle GetDiffuseMap() const;
		void SetDiffuseMap(Texture2DHandle diffuse);

		Texture2DHandle GetSpecularMap() const;
		void SetSpecularMap(Texture2DHandle specular);

		const Vector4& GetColor() const;
		Vector4& GetColor();
		void SetColor(const Vector4& color);

		static MaterialHandle DefaultMat();
		static MaterialHandle SpriteMat();
		static MaterialHandle MissingMat();

		bool operator==(const Material& other) const;
		bool operator!=(const Material& other) const;

	private:
		ShaderHandle m_shader;
		Texture2DHandle m_diffuseMap;
		Texture2DHandle m_specularMap;
		Vector4 m_color;

		ASinglyLinkedList<MatUniform> m_additionalUniforms;
	};

	/*struch which contains the data to
	  send to the gpu for each vertex

	  the order of the fields is the layout to be sent to the gpu
	*/
	struct VertexData
	{
		Vector3 position;
		//Vector3 textureCoords;
		//Vector3 normal;
		//Vector4 color;
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
			return position == other.position; /* && textureCoords == other.textureCoords
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

		void Reset()
		{
			numDrawCalls = 0;
			numVertices = 0;
			numIndices = 0;
		}
	};

	//one batch per render target (triangles, lines, points, etc.)
	class RenderingBatch
	{
		friend class Renderer;
	public:
		RenderingBatch();
		RenderingBatch(const RenderingBatch& other);
		~RenderingBatch();

		void Add(const VertexData* vertices, unsigned int numVertices, const unsigned int* indices,
			unsigned int numIndices, RenderingPrimitive renderTarget);

		//returns texture index for the texture
		int AddTexture2D(AReference<Texture2D> texture, RenderingPrimitive renderTarget);

		//returns texture index for the texture
		int AddCubemap(AReference<CubeMap> cubemap, RenderingPrimitive renderTarget);

		void AddTexture2DShadowMap(AReference<Texture2D> shadowMap);

		void AddCubemapShadowMap(AReference<CubeMap> shadowMap);

		void Draw(RenderingPrimitive renderTarget);

		bool IsEmpty() const;

	private:
		void ResetBatch();

		static unsigned int s_maxVertices;
		static unsigned int s_maxIndices;
		static unsigned int s_maxTextureSlots;
		static unsigned int s_maxTexture2DSlots;
		static unsigned int s_maxTexture2DShadowMapSlots;
		static unsigned int s_maxCubemapSlots;
		static unsigned int s_maxCubemapShadowMapSlots;


		AReference<VertexBuffer> m_vbo;
		AReference<IndexBuffer> m_ibo;

		VertexData* m_vertexDataArr;
		unsigned int m_vertexDataIndex = 0;

		unsigned int* m_indicesArr;
		unsigned int m_indicesIndex = 0;

		AReference<Texture2D>* m_texture2DSlots;
		AReference<Texture2D>* m_texture2DShadowMapSlots;
		AReference<CubeMap>* m_cubemapSlots;
		AReference<CubeMap>* m_cubemapShadowMapSlots;
		unsigned int m_texture2DIndex = 0;
		unsigned int m_cubemapIndex = 0;
		unsigned int m_texture2DShadowMapIndex = 0;
		unsigned int m_cubemapShadowMapIndex = 0;
	};

	class Renderer
	{
		friend class RenderingBatch;
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

		//draw generic Vertex Data
		static void DrawVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Vector3* vertices,
			unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount, 
			AReference<Texture2D> texture, const Vector3* textureCoords, float tileFactor, const Vector4& tintColor);

		static void DrawVertexData(RenderingPrimitive renderTarget, const Vector3& position, const Quaternion& rotation,
			const Vector3& scale, const Vector3* vertices, unsigned int numVertices, const Vector3* normals, 
			unsigned int* indices, unsigned int indexCount, AReference<Texture2D> texture, const Vector3* textureCoords,
			float tileFactor, const Vector4& tintColor);

		//lighting functions
		static void AddDirectionalLight(const Vector3& position, const Vector3& direction,
			const Vector4& lightColor = { 1, 1, 1, 1 }, bool drawCubes = true);

		static void AddPointLight(const Vector3& position, const Vector4& lightColor = { 1, 1, 1, 1 },
			bool drawCubes = true);

		//sprite
		static void DrawSprite(AEntity e, const SpriteRenderer& sprite);

		static void DrawSprite(const Vector3& position, float rotation, const Vector2& size,
			const SpriteRenderer& sprite);

		//mesh
		static void DrawMesh(AEntity e, MaterialHandle material, const MeshRenderer& mesh);
		static void DrawMesh(AEntity e, const MeshRenderer& mesh);

		//UI
		static void DrawUIElement(const UIElement& element, const Vector4& color);

	private:
		//draw to data passed to the renderer to the screen
		static void FlushBatch();

		static void CleanUpAfterShadowMapGeneration();

		static void AddShadowMapToShaders();

		static void GenerateShadowMaps();

		//helper function which loads a voxel into the data to pass to the gpu when the renderer flushes
		static void UploadVoxel(const Mat4& transform, const Material& mat, AReference<CubeMap> texture,
			float tileFactor, const Vector4& tintColor);

		//uploads a quad or filled in square into the renderer
		static void UploadQuad(const Mat4& transform, MaterialHandle mat, Texture2DHandle texture,
			float tileFactor, const Vector4& tintColor, bool ignoresCam);

		//helper function which allows to pass any vertex data with a 2D texture
		static void UploadVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Material& mat,
			const Vector3* vertices, unsigned int numVertices, const Vector3* normals, unsigned int* indices,
			unsigned int indexCount, AReference<Texture2D> texture, const Vector3* textureCoords, 
			float tileFactor, const Vector4& tintColor);

		static void UploadMesh(const Mat4& transform, const Material& mat, AReference<Mesh>& mesh,
			AReference<Texture2D> texture, float tileFactor = 1, const Vector4& tintColor = { 1, 1, 1, 1 });

		static RendererStatistics s_stats;
		static RenderingDataSorter s_sorter;
		static Mat4 s_viewProjMatrix;

	};
}