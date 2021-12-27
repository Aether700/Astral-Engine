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

	class DrawCommand;

	class Material
	{
	public:
		Material(float ambiant = 0.3f, float diffuse = 0.5f, float specular = 1.0f,
			float shininess = 64.0f)
			: m_ambiantIntensity(ambiant), m_diffuseIntensity(diffuse),
			m_specularIntensity(specular), m_shininess(shininess),
			m_ignoresLighting(0.0f) { }

		Material(bool ignoresLighting)
			: m_ambiantIntensity(0.3f), m_diffuseIntensity(0.5f),
			m_specularIntensity(1.0f), m_shininess(64.0f),
			m_ignoresLighting(ignoresLighting ? 1.0f : 0.0f) { }

		bool operator==(const Material& other) const
		{
			return m_ambiantIntensity == other.m_ambiantIntensity
				&& m_diffuseIntensity == other.m_diffuseIntensity
				&& m_specularIntensity == other.m_specularIntensity
				&& m_shininess == other.m_shininess;
		}

		bool operator!=(const Material& other) const
		{
			return !(*this == other);
		}

	private:
		float m_ambiantIntensity;
		float m_diffuseIntensity;
		float m_specularIntensity;
		float m_shininess;
		float m_ignoresLighting;
	};

	/*struch which contains the data to
	  send to the gpu for each vertex

	  the order of the fields is the layout to be sent to the gpu
	*/
	struct VertexData
	{
		Vector3 position;
		Vector3 textureCoords;
		Vector3 normal;
		Vector4 color;
		float textureIndex;
		float tillingFactor;
		float uses3DTexture;
		float ignoresCamera;
		Material mat;

		bool operator==(const VertexData& other) const
		{
			return position == other.position && textureCoords == other.textureCoords
				&& normal == other.normal
				&& color == other.color
				&& textureIndex == other.textureIndex
				&& tillingFactor == other.tillingFactor
				&& mat == other.mat;
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
		int AddTexture2D(const AReference<Texture2D>& texture, RenderingPrimitive renderTarget);

		//returns texture index for the texture
		int AddCubemap(AReference<CubeMap> cubemap, RenderingPrimitive renderTarget);

		void AddTexture2DShadowMap(const AReference<Texture2D>& shadowMap);

		void AddCubemapShadowMap(const AReference<CubeMap>& shadowMap);

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
		friend class DrawCommand;
	public:
		static void Init();
		static void Shutdown();

		static const RendererStatistics& GetStats();
		static void ResetStats();

		//use to start renderering and stop rendering
		static void BeginScene();
		static void BeginScene(const Mat4& viewProjMatrix, const Vector3& camPos);
		static void BeginScene(const OrthographicCamera& cam);
		static void BeginScene(const RuntimeCamera& cam);
		static void BeginScene(const RuntimeCamera& camera, const Transform& transform);
		static void EndScene();

		//requests that all the light's shadow maps be recalculated for this frame
		static void UpdateLights();

		//turn on and off the shadows in the scene
		static void UseShadows(bool value) { s_useShadows = value; }

		static const AReference<CubeMap>& GetDefaultWhiteCubeMap();
		static const AReference<Texture2D>& GetDefaultWhiteTexture();

		//primitives

		//voxels/cubes
		static void DrawVoxel(const Mat4& transform, const Material& mat, AReference<CubeMap> texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawVoxel(const Mat4& transform, AReference<CubeMap> texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Material& mat, AReference<CubeMap> texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			AReference<CubeMap> texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawVoxel(const Mat4& transform, const Material& mat,
			const Vector4& color = { 1, 1, 1, 1 });

		static void DrawVoxel(const Mat4& transform,
			const Vector4& color = { 1, 1, 1, 1 });

		static void DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Material& mat, const Vector4& tintColor = { 1, 1, 1, 1 });

		static void DrawVoxel(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Vector4& tintColor = { 1, 1, 1, 1 });

		//squares
		static void DrawQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Mat4& transform, const AReference<Texture2D>& texture,
			float tileFactor = 1.0f, const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Material& mat, const AReference<Texture2D>& texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const AReference<Texture2D>& texture, float tileFactor = 1.0f,
			const Vector4& tintColor = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Mat4& transform, const Material& mat, 
			const Vector4& color = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Mat4& transform, const Vector4& color = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Material& mat, const Vector4& color = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, const Vector3& rotation, const Vector3& scale,
			const Vector4& color = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, float rotation, const Vector2& scale, 
			const AReference<Texture2D>& texture, float tilingFactor = 1, const Vector4& color = { 1, 1, 1, 1 }, 
			bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, float rotation, const Vector3& scale,
			const Vector4& color = { 1, 1, 1, 1 }, bool ignoresCam = false);

		static void DrawQuad(const Vector3& position, float rotation, const Vector3& scale, 
			const AReference<Texture2D>& texture, const Vector2* textureCoords, float tilingFactor = 1, 
			const Vector4& tintColor = { 1, 1, 1, 1 },	bool ignoresCam = false);

		//draw generic Vertex Data
		static void DrawVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Vector3* vertices,
			unsigned int numVertices, const Vector3* normals, unsigned int* indices, unsigned int indexCount, 
			const AReference<Texture2D>& texture, const Vector3* textureCoords, float tileFactor, const Vector4& tintColor);

		static void DrawVertexData(RenderingPrimitive renderTarget, const Vector3& position, const Vector3& rotation,
			const Vector3& scale, const Vector3* vertices, unsigned int numVertices, const Vector3* normals, 
			unsigned int* indices, unsigned int indexCount, const AReference<Texture2D>& texture, const Vector3* textureCoords,
			float tileFactor, const Vector4& tintColor);

		static void DrawMesh(const Mat4& transform, AReference<Mesh>& mesh, const AReference<Texture2D>& texture,
			float tileFactor = 1, const Vector4& tintColor = { 1, 1, 1, 1 });

		//lighting functions
		static void AddDirectionalLight(const Vector3& position, const Vector3& direction,
			const Vector4& lightColor = { 1, 1, 1, 1 }, bool drawCubes = true);

		static void AddPointLight(const Vector3& position, const Vector4& lightColor = { 1, 1, 1, 1 },
			bool drawCubes = true);

		//sprite
		static void DrawSprite(const Mat4& transform, const SpriteRenderer& sprite, bool ignoresCam = false);

		static void DrawSprite(const Vector3& position, float rotation, const Vector2& size,
			const SpriteRenderer& sprite, bool ignoresCam = false);

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
		static void UploadQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
			float tileFactor, const Vector4& tintColor, bool ignoresCam);
		static void UploadQuad(const Mat4& transform, const Material& mat, const AReference<Texture2D>& texture,
			const Vector3* textureCoords, float tileFactor, const Vector4& tintColor, bool ignoresCam);

		//helper function which allows to pass any vertex data with a 2D texture
		static void UploadVertexData(RenderingPrimitive renderTarget, const Mat4& transform, const Material& mat,
			const Vector3* vertices, unsigned int numVertices, const Vector3* normals, unsigned int* indices,
			unsigned int indexCount, const AReference<Texture2D>& texture, const Vector3* textureCoords,
			float tileFactor, const Vector4& tintColor);

		static void UploadMesh(const Mat4& transform, const Material& mat, AReference<Mesh>& mesh,
			const AReference<Texture2D>& texture, float tileFactor = 1, const Vector4& tintColor = { 1, 1, 1, 1 });

		static RendererStatistics s_stats;

		static AReference<CubeMap> s_defaultWhiteCubeMap;
		static AReference<Texture2D> s_defaultWhiteTexture;
		static AReference<Shader> s_shader;
		static Material s_defaultMaterial;
		static AUnorderedMap<RenderingPrimitive, RenderingBatch>* s_renderingBatches;
		static bool s_useShadows;

		static DirectionalLight* s_directionalLightArr;
		static unsigned int s_directionalLightIndex;

		static PointLight* s_pointLightArr;
		static unsigned int s_pointLightIndex;

		static bool s_updateLights;

		static ADynArr<DrawCommand> s_drawCommands; //keeps track of the draw commands executed this frame

	};
}