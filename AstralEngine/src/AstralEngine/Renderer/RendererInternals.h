#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/AEntity.h"
#include "Renderer.h"
#include "Framebuffer.h"


namespace AstralEngine
{
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

		AReference<Shader> PrepareForRender(const Mat4& viewProjMatrix);
		void BindTexureData();

	private:
		static constexpr unsigned int s_framebufferWidth = 512;
		static constexpr unsigned int s_framebufferHeight = 512;

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

		void AddData(DrawCommand* data);
		void Draw(const Mat4& viewProj);
		void Clear();

		void BindGBufferTextureData();

	private:
		void SendLightUniformsToShader(AReference<Shader>& shader);
		void SetupFullscreenRenderingObjects();

		GBuffer* m_gBuffer;
		RenderingDataSorter m_opaque;
		RenderingDataSorter* m_transparent;

		ShaderHandle m_deferredShader;
		AReference<VertexBuffer> m_deferredVB;
		AReference<IndexBuffer> m_deferredIB;
	};
}