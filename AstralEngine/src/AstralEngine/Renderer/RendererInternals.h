#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/AEntity.h"
#include "Renderer.h"


namespace AstralEngine
{
	struct VertexData;
	struct InstanceVertexData;

	class DrawCommand
	{
	public:
		DrawCommand();
		DrawCommand(const Mat4& transform, MaterialHandle mat, MeshHandle mesh, const Vector4& color, const AEntity e);

		const Mat4& GetTransform() const;
		MaterialHandle GetMaterial() const;
		MeshHandle GetMesh() const;
		const Vector4& GetColor() const;
		AEntity GetEntity() const;
		bool IsOpaque() const;

		bool operator==(const DrawCommand& other) const;
		bool operator!=(const DrawCommand& other) const;

	private:
		Mat4 m_transform;
		Vector4 m_color;
		MeshHandle m_mesh;
		MaterialHandle m_material;
		AEntity m_entity;
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
		void AddDrawCommand(DrawCommand* draw);
		void Clear();

		void TempRenderFunc(MeshHandle mesh);

	private:
		void CollectMeshesToInstance(ASinglyLinkedList<MeshHandle>& toInstance);
		void AddToBatching(DrawCommand* cmd);
		void RenderBatch(const Mat4& viewProj);
		void RenderInstancing(const Mat4& viewProj, AUnorderedMap<MeshHandle, 
			ASinglyLinkedList<DrawCommand*>> commandsToInstance);
		void RenderMeshInstance(const Mat4& viewProj, MeshHandle mesh, ASinglyLinkedList<DrawCommand*>& commands);

		void InstanceRenderMeshSection(VertexData* vertexData, size_t numVertex, 
			const ADynArr<unsigned int>& indices, InstanceVertexData* instanceData, 
			size_t numInstanceData, size_t dataOffset, size_t drawCallSize);

		void ReadVertexDataFromMesh(AReference<Mesh>& mesh, VertexData* vertexDataArr, size_t dataOffset,
			size_t dataCount);

		// once a mesh has been used s_instancingCutoff times or 
		// more in a single frame it will be sent to be instanced instead of being batched 
		// with the rest of the data
		static constexpr size_t s_instancingCutoff = 1;//10; 
		static size_t s_maxNumVertex;
		static size_t s_maxNumIndices;

		// used for batching
		AReference<VertexBuffer> m_batchBuffer;
		AReference<IndexBuffer> m_batchIndices;
		
		VertexData* m_batchDataArr;
		size_t m_batchDataArrCount;
		size_t m_batchDataArrMaxCount;

		unsigned int* m_batchIndicesArr;
		size_t m_batchIndicesArrCount;
		size_t m_batchIndicesArrMaxCount;

		// used for instancing
		AReference<VertexBuffer> m_instancingBuffer;
		AReference<VertexBuffer> m_instancingArr;
		AReference<IndexBuffer> m_instancingIndices;

		// keeps track of how many time each mesh has been used this frame
		AUnorderedMap<MeshHandle, size_t> m_meshUseCounts; 

		ASinglyLinkedList<DrawCommand*> m_drawCommands;
	};

	// lists all the data to be drawn to the screen in one or more draw calls
	class DrawCallList
	{

		// Eventually will need to keep track of what matrix is where in the m_instanceArrBuffer 
		// and only change the ones which have been modified

		// might want to check that in another object not sure yet
	public:
		DrawCallList();
		DrawCallList(MaterialHandle material, MeshHandle type);
		~DrawCallList();

		MaterialHandle GetMaterial() const;
		MeshHandle GetMesh() const;

		void Draw(const Mat4& viewProj) const;

		void AddDrawCommand(DrawCommand* draw);
		void Clear();

	private:
		void SetupGeometryData();
		void SetupQuad();

		ASinglyLinkedList<DrawCommand*> m_data;
		MaterialHandle m_material;
		MeshHandle m_mesh;

		AReference<VertexBuffer> m_geometryDataBuffer;
		AReference<VertexBuffer> m_instanceArrBuffer;
		AReference<IndexBuffer> m_indexBuffer;
	};

	class RenderingDataSorter
	{
	public:
		using AIterator = AUnorderedMap<MaterialHandle, DrawDataBuffer>::AIterator;
		using AConstIterator = AUnorderedMap<MaterialHandle, DrawDataBuffer>::AConstIterator;

		void AddData(DrawCommand* data);
		void Clear();

		AIterator begin();
		AIterator end();

		AConstIterator begin() const;
		AConstIterator end() const;

	private:
		AUnorderedMap<MaterialHandle, DrawDataBuffer> m_buffers;
	};

	// ordered list of DrawCommands. Used to sort in what order what should be drawn
	class DrawList
	{
	public:
		~DrawList();

		//think over how draw cmds are stored/sorted internally and then implement draw list class
		//need to group similar meshes/vertex data together to use instance rendering

		void AddDrawCmd(DrawCommand* cmd);
		DrawCommand* GetNextDrawCmd();
		bool IsEmpty() const;

	private:
		//ADoublyLinkedList<DrawCommand*> m_drawCommands;
	};
}