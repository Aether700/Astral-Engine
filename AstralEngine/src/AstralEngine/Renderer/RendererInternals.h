#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/AEntity.h"
#include "Renderer.h"
#include "Framebuffer.h"


namespace AstralEngine
{
	typedef size_t LightHandle;

	struct VertexData;
	struct InstanceVertexData;
	struct BatchedVertexData;

	class DrawCommand
	{
	public:
		DrawCommand();
		DrawCommand(const Mat4& transform, MaterialHandle mat, MeshHandle mesh, 
			const Vector4& color, const AEntity e, bool opaque, Texture2DHandle texture = NullHandle);

		const Mat4& GetTransform() const;
		MaterialHandle GetMaterial() const;
		MeshHandle GetMesh() const;
		const Vector4& GetColor() const;
		AEntity GetEntity() const;
		Texture2DHandle GetTexture() const;
		bool IsOpaque() const;
		bool UsesDeferred() const;

		bool operator==(const DrawCommand& other) const;
		bool operator!=(const DrawCommand& other) const;

	private:
		Mat4 m_transform;
		Vector4 m_color;
		MeshHandle m_mesh;
		MaterialHandle m_material;
		AEntity m_entity;
		Texture2DHandle m_texture;
		bool m_opaque;
	};


	class GBuffer
	{
	public:
		GBuffer();
		void Bind();
		void Unbind();

		void OnWindowResize(WindowResizeEvent& resize);

		const AReference<Framebuffer>& GetFramebuffer() const;

		AReference<Shader> PrepareForRender(const Mat4& viewProjMatrix);
		void BindTexureData();

	private:
		AReference<Framebuffer> m_framebuffer;
	};

	// class responsible for handling draw calls by either batching them or instanciating them
	// additionally this class will keep track of what transforms matrix have not changed since 
	// last frame and will only update matrices which have changed
	class DrawDataBuffer sealed
	{
	public:
		DrawDataBuffer();
		~DrawDataBuffer();

		void Initialize();

		void Draw(const Mat4& viewProj, MaterialHandle material);
		void RenderGeometry(const Mat4& viewProj);
		void AddDrawCommand(DrawCommand* draw);
		void Clear();

		bool IsEmpty() const;

	private:
		void ReadVertexDataFromMesh(AReference<Mesh>& mesh, VertexData* vertexDataArr, size_t dataOffset,
			size_t dataCount);

		// returns -1 if there is no more available texture slots
		int GetTextureIndex(Texture2DHandle* arr, size_t& index, Texture2DHandle texture);
		void BindTextures(Texture2DHandle* arr, size_t index);

		size_t ComputeDrawCallSize();
		// Batching /////////////////////////////////////////////
		void AddToBatching(const Mat4& viewProj, DrawCommand* cmd);
		void RenderBatch(const Mat4& viewProj);
		void ClearBatching();
		void BatchRenderMeshSection(BatchedVertexData* vertexData, size_t numVertex,
			const ADynArr<unsigned int>& indices, size_t dataOffset, size_t drawCallSize);


		// Instancing ////////////////////////////////////////////
		void CollectMeshesToInstance(ASinglyLinkedList<MeshHandle>& toInstance);
		void RenderInstancing(const Mat4& viewProj, AUnorderedMap<MeshHandle, 
			ADynArr<DrawCommand*>>& commandsToInstance);
		void RenderMeshInstance(const Mat4& viewProj, MeshHandle mesh, ADynArr<DrawCommand*>& commands);

		void InstanceRenderMeshSection(VertexData* vertexData, size_t numVertex, 
			const ADynArr<unsigned int>& indices, InstanceVertexData* instanceData, 
			size_t numInstanceData, size_t dataOffset, size_t drawCallSize);
		void ClearInstancing();


		// once a mesh has been used s_instancingCutoff times or 
		// more in a single frame it will be sent to be instanced instead of being batched 
		// with the rest of the data
		static constexpr size_t s_instancingCutoff = 10;//1000; 
		static size_t s_maxNumVertex;
		static size_t s_maxNumIndices;
		static size_t s_numTextureSlots;

		// used for batching
		AReference<VertexBuffer> m_batchBuffer;
		AReference<IndexBuffer> m_batchIndices;
		
		BatchedVertexData* m_batchDataArr;
		size_t m_batchDataArrIndex;

		unsigned int* m_batchIndicesArr;
		size_t m_batchIndicesArrIndex;

		Texture2DHandle* m_batchTextureSlots;
		size_t m_batchTextureSlotIndex;
		bool m_hasBatchedData;

		// used for instancing
		AReference<VertexBuffer> m_instancingBuffer;
		AReference<VertexBuffer> m_instancingArr;
		AReference<IndexBuffer> m_instancingIndices;

		Texture2DHandle* m_instancingTextureSlots;
		size_t m_instancingTextureSlotIndex;

		// keeps track of how many time each mesh has been used this frame
		AUnorderedMap<MeshHandle, size_t> m_meshUseCounts; 
		AUnorderedMap<MeshHandle, ADynArr<DrawCommand*>> m_commandsToInstance;
		AUnorderedMap<MeshHandle, ADynArr<DrawCommand*>> m_commandsToBatch;

		ASinglyLinkedList<DrawCommand*> m_drawCommands;
	};

	
	class RenderingDataSorter
	{
	public:
		using AIterator = AUnorderedMap<MaterialHandle, DrawDataBuffer>::AIterator;
		using AConstIterator = AUnorderedMap<MaterialHandle, DrawDataBuffer>::AConstIterator;

		void AddData(DrawCommand* data);
		void Clear();

		bool IsEmpty() const;

		AIterator begin();
		AIterator end();

		AConstIterator begin() const;
		AConstIterator end() const;

	private:
		AUnorderedMap<MaterialHandle, DrawDataBuffer> m_buffers;
	};

	// processes and renders to the screen according to a specific rendering path either forward or deferred
	class RenderQueue sealed
	{
	public:
		RenderQueue(GBuffer* gBuffer = nullptr);
		~RenderQueue();

		void OnWindowResize(WindowResizeEvent& resize);

		void AddData(DrawCommand* data);
		void Draw(const Mat4& viewProj);
		void Clear();

		void BindGBufferTextureData();

	private:
		void SetupFullscreenRenderingObjects();

		GBuffer* m_gBuffer;
		RenderingDataSorter m_opaque;
		RenderingDataSorter* m_transparent;

		ShaderHandle m_deferredShader;
		AReference<VertexBuffer> m_deferredVB;
		AReference<IndexBuffer> m_deferredIB;
	};


	class LightData
	{
	public:
		LightData();
		LightData(const Vector3& position, const Vector3& color = { 1.0f, 1.0f, 1.0f });

		LightType GetLightType() const;
		const Vector3& GetPosition() const;
		const Vector3& GetDirection() const;
		const Vector3& GetColor() const;
		const Vector3& GetAmbientColor() const;
		const Vector3& GetDiffuseColor() const;
		const Vector3& GetSpecularColor() const;
		float GetAmbientIntensity() const;
		float GetDiffuseIntensity() const;
		float GetSpecularIntensity() const;
		float GetRadius() const;

		void SetLightType(LightType type);
		void SetPosition(const Vector3& position);
		void SetDirection(const Vector3& position);
		void SetColor(const Vector3& color);
		void SetAmbientIntensity(float intensity);
		void SetDiffuseIntensity(float intensity);
		void SetSpecularIntensity(float intensity);
		void SetRadius(float radius);

	private:
		LightType m_type;
		Vector3 m_position;
		Vector3 m_direction;
		Vector3 m_color;
		float m_ambientIntensity;
		float m_diffuseIntensity;
		float m_specularIntensity;

		float m_radius;
		/*
		// used for attenuation
		float m_constantTerm;
		float m_linearTerm;
		float m_quadraticTerm;
		*/
	};

	class LightHandler sealed
	{
		friend class Light;
		friend class Renderer;
	public:
		LightHandler();
		LightHandle AddLight(LightData& data);
		void RemoveLight(LightHandle light);

		LightData& GetLightData(LightHandle light);
		const LightData& GetLightData(LightHandle light) const;

		bool LightIsValid(LightHandle light) const;
		bool LightsModified() const;

		static constexpr size_t GetMaxNumLights() { return s_maxNumLights; }

		void SendLightUniformsToShader(AReference<Shader>& shader) const;

	private:
		static constexpr size_t s_maxNumLights = 50;

		void SendDirectionalLightUniforms(AReference<Shader>& shader) const;
		void SendPointLightUniforms(AReference<Shader>& shader) const;
		void OnLightTypeChange(LightHandle light, LightType oldType, LightType newType);

		ADynArr<LightData> m_lights;
		AStack<LightHandle> m_handlesToRecycle;
		ASinglyLinkedList<LightHandle> m_directionalLights;
		ASinglyLinkedList<LightHandle> m_pointLights;
		bool m_lightsModified;
	};
}