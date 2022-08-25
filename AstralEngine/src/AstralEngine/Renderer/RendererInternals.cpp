#include "aepch.h"
#include "RendererInternals.h"
#include "Mesh.h"

namespace AstralEngine
{
	struct InstanceVertexData
	{
		Mat4 transform;
		Vector4 color;
		float textureIndex;
	};

	struct BatchedVertexData
	{
		VertexData vertex;
		InstanceVertexData instance;
	};

	// DrawCommand /////////////////////////////////
	
	DrawCommand::DrawCommand() { }

	DrawCommand::DrawCommand(const Mat4& transform, MaterialHandle mat, MeshHandle mesh, const Vector4& color, 
		const AEntity e, Texture2DHandle texture) : m_transform(transform), m_mesh(mesh), m_material(mat), 
		m_color(color), m_entity(e), m_texture(texture)
	{
		if (m_material == NullHandle)
		{
			m_material = Material::MissingMat();
		}
	}

	const Mat4& DrawCommand::GetTransform() const { return m_transform; }
	MaterialHandle DrawCommand::GetMaterial() const { return m_material; }
	MeshHandle DrawCommand::GetMesh() const { return m_mesh; }
	const Vector4& DrawCommand::GetColor() const { return m_color; }
	AEntity DrawCommand::GetEntity() const { return m_entity; }
	
	Texture2DHandle DrawCommand::GetTexture() const { return m_texture; }

	bool DrawCommand::IsOpaque() const 
	{ 
		return ResourceHandler::GetMaterial(m_material)->GetColor().a == 1.0f && m_color.a == 1.0f; 
	}

	bool DrawCommand::operator==(const DrawCommand& other) const
	{
		return m_mesh == other.m_mesh && m_texture == other.m_texture 
			&& m_material == other.m_material && m_transform == other.m_transform;
	}

	bool DrawCommand::operator!=(const DrawCommand& other) const
	{
		return !(*this == other);
	}

	// DrawDataBuffer ////////////////////////////////////////////////////

	size_t DrawDataBuffer::s_maxNumVertex = 0;
	size_t DrawDataBuffer::s_maxNumIndices;
	size_t DrawDataBuffer::s_numTextureSlots;


	DrawDataBuffer::DrawDataBuffer()
	{
		m_batchDataArr = nullptr;
		m_batchIndicesArr = nullptr;
		m_batchTextureSlots = nullptr;
	}

	DrawDataBuffer::~DrawDataBuffer()
	{
		delete[] m_batchDataArr;
		delete[] m_batchIndicesArr;
		delete[] m_batchTextureSlots;
	}

	void DrawDataBuffer::Initialize()
	{
		if (s_maxNumVertex == 0)
		{
			s_maxNumVertex = RenderCommand::GetMaxNumVertices();
			s_maxNumIndices = RenderCommand::GetMaxNumIndices();
			s_numTextureSlots = RenderCommand::GetNumTextureSlots();
		}

		// Batching
		m_batchBuffer = VertexBuffer::Create(s_maxNumVertex * sizeof(VertexData));;
		m_batchIndices = IndexBuffer::Create();

		m_batchBuffer->Bind();
		m_batchBuffer->SetLayout({ 
			{ ADataType::Float3, "position" },
			{ ADataType::Float2, "textureCoords" },
			{ ADataType::Mat4, "transform" },
			{ ADataType::Float4, "color" },
			{ ADataType::Int, "textureIndex" }
			});

		m_batchDataArr = new BatchedVertexData[s_maxNumVertex];
		m_batchDataArrIndex = 0;

		m_batchIndicesArr = new unsigned int[s_maxNumIndices];
		m_batchIndicesArrIndex = 0;

		m_batchTextureSlots = new Texture2DHandle[s_numTextureSlots];
		m_batchTextureSlotIndex = 0;

		// Instancing
		m_instancingBuffer = VertexBuffer::Create(s_maxNumVertex * sizeof(VertexData));
		m_instancingArr = VertexBuffer::Create(s_maxNumVertex * sizeof(InstanceVertexData), true);
		m_instancingIndices = IndexBuffer::Create();

		m_instancingBuffer->Bind();
		m_instancingBuffer->SetLayout({
			{ ADataType::Float3, "position" },
			{ ADataType::Float2, "textureCoords" }
			});
		
		m_instancingArr->Bind();
		m_instancingArr->SetLayout({
			{ ADataType::Mat4, "transform", false, 1 },
			{ ADataType::Float4, "color", false, 1 },
			{ ADataType::Int, "textureIndex", false, 1 }
			}, 2);

		m_instancingTextureSlots = new Texture2DHandle[s_numTextureSlots];
		m_instancingTextureSlotIndex = 0;
	}

	void DrawDataBuffer::Draw(const Mat4& viewProj, MaterialHandle material)
	{
		ASinglyLinkedList<MeshHandle> meshesToInstance;
		AUnorderedMap<MeshHandle, ASinglyLinkedList<DrawCommand*>> commandsToInstance;
		CollectMeshesToInstance(meshesToInstance);

		AReference<Material> mat = ResourceHandler::GetMaterial(material);
		AE_CORE_ASSERT(mat != nullptr, "");

		AReference<Shader> shader = ResourceHandler::GetShader(mat->GetShader());
		AE_CORE_ASSERT(shader != nullptr, "");

		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", viewProj);
		mat->SendUniformsToShader();

		for (DrawCommand* cmd : m_drawCommands)
		{
			//if (meshesToInstance.Contains(cmd->GetMesh()))
			if (true)
			{
				if (commandsToInstance.ContainsKey(cmd->GetMesh()))
				{
					commandsToInstance[cmd->GetMesh()].Add(cmd);
				}
				else
				{
					commandsToInstance.Add(cmd->GetMesh(), ASinglyLinkedList<DrawCommand*>());
					commandsToInstance[cmd->GetMesh()].Add(cmd);
				}
			}
			else
			{
				AddToBatching(viewProj, cmd);
			}
		}

		RenderBatch(viewProj);
		RenderInstancing(viewProj, commandsToInstance);
	}

	void DrawDataBuffer::AddDrawCommand(DrawCommand* draw)
	{
		AE_CORE_ASSERT(draw->GetMesh() != NullHandle, "");
		m_drawCommands.Add(draw);
		if (m_meshUseCounts.ContainsKey(draw->GetMesh()))
		{
			m_meshUseCounts[draw->GetMesh()]++;
		}
		else
		{
			m_meshUseCounts.Add(draw->GetMesh(), 1);
		}
	}

	void DrawDataBuffer::Clear()
	{
		for (DrawCommand* cmd : m_drawCommands)
		{
			delete cmd;
		}

		m_drawCommands.Clear();
		m_meshUseCounts.Clear();
		ClearBatching();
		m_instancingTextureSlotIndex = 0;
	}

	void DrawDataBuffer::ReadVertexDataFromMesh(AReference<Mesh>& mesh, VertexData* vertexDataArr,
		size_t dataOffset, size_t dataCount)
	{
		const ADynArr<Vector3>& positions = mesh->GetPositions();
		const ADynArr<Vector2>& textureCoords = mesh->GetTextureCoords();
		for (size_t i = 0; i < positions.GetCount(); i++)
		{
			vertexDataArr[dataOffset + i].position = positions[i];
			vertexDataArr[dataOffset + i].textureCoords = textureCoords[i];
		}
	}

	int DrawDataBuffer::GetTextureIndex(Texture2DHandle* arr, size_t& index, Texture2DHandle texture)
	{
		if (texture == NullHandle)
		{
			return -1;
		}

		AE_CORE_ASSERT(index < s_numTextureSlots, "");
		for (size_t i = 0; i < index; i++)
		{
			if (arr[i] == texture)
			{
				return (int)i;
			}
		}

		arr[index] = texture;
		int i = (int)index;
		index++;
		return i;
	}

	void DrawDataBuffer::BindTextures(Texture2DHandle* arr, size_t index)
	{
		for (size_t i = 0; i < index; i++)
		{
			AReference<Texture2D> texture = ResourceHandler::GetTexture2D(arr[i]);
			texture->Bind(i);
		}
	}

	size_t DrawDataBuffer::ComputeDrawCallSize()
	{
		// make sure numVertex is a multiple of 3 since we are drawing triangles
		size_t maxVertexSize = s_maxNumVertex - (s_maxNumVertex % 3);
		size_t maxIndexSize = s_maxNumIndices - (s_maxNumIndices % 3);

		return Math::Min(maxVertexSize, maxIndexSize);
	}

	void DrawDataBuffer::AddToBatching(const Mat4& viewProj, DrawCommand* cmd)
	{
		AReference<Mesh> mesh = ResourceHandler::GetMesh(cmd->GetMesh());
		AE_CORE_ASSERT(mesh != nullptr, "");
		const ADynArr<Vector3>& positions = mesh->GetPositions();
		const ADynArr<Vector2>& textureCoords = mesh->GetTextureCoords();
		const ADynArr<unsigned int>& indices = mesh->GetIndices();
		int textureIndex = GetTextureIndex(m_batchTextureSlots, m_batchTextureSlotIndex, cmd->GetTexture());

		if (positions.GetCount() >= s_maxNumVertex || indices.GetCount() >= s_maxNumIndices)
		{
			// split up mesh for render here
			size_t numVertices = positions.GetCount();
			BatchedVertexData* vertexDataArr = new BatchedVertexData[numVertices];


			for (size_t i = 0; i < numVertices; i++)
			{
				vertexDataArr[i].vertex.position = positions[i];
				vertexDataArr[i].vertex.textureCoords = textureCoords[i];
				vertexDataArr[i].instance.color = cmd->GetColor();
				vertexDataArr[i].instance.transform = cmd->GetTransform();
				vertexDataArr[i].instance.textureIndex = (float)textureIndex;
			}

			size_t drawCallSize = ComputeDrawCallSize();
			size_t offset = 0;
			size_t currDrawSize = indices.GetCount();
			while (offset < indices.GetCount())
			{
				RenderBatch(viewProj);
				ClearBatching();

				currDrawSize = Math::Min(drawCallSize, indices.GetCount() - offset);
				BatchRenderMeshSection(vertexDataArr, numVertices, indices, offset, currDrawSize);
				offset += currDrawSize;
			}

			m_batchDataArrIndex = currDrawSize;
			m_batchIndicesArrIndex = currDrawSize;

			delete[] vertexDataArr;
			return;

		}
		else if (m_batchDataArrIndex + positions.GetCount() >= s_maxNumVertex 
			|| m_batchIndicesArrIndex + indices.GetCount() >= s_maxNumIndices )
		{
			RenderBatch(viewProj);
			ClearBatching();
		}

		for (size_t i = 0; i < positions.GetCount(); i++)
		{
			m_batchDataArr[m_batchDataArrIndex + i].vertex.position = positions[i];
			m_batchDataArr[m_batchDataArrIndex + i].vertex.textureCoords = textureCoords[i];
			m_batchDataArr[m_batchDataArrIndex + i].instance.transform = cmd->GetTransform();
			m_batchDataArr[m_batchDataArrIndex + i].instance.color = cmd->GetColor();
			m_batchDataArr[m_batchDataArrIndex + i].instance.textureIndex = (float)textureIndex;
		}

		for (size_t i = 0; i < indices.GetCount(); i++)
		{
			m_batchIndicesArr[m_batchIndicesArrIndex + i] = m_batchDataArrIndex + indices[i];
		}

		m_batchDataArrIndex += positions.GetCount();
		m_batchIndicesArrIndex += indices.GetCount();
	}

	void DrawDataBuffer::RenderBatch(const Mat4& viewProj)
	{
		BindTextures(m_batchTextureSlots, m_batchTextureSlotIndex);

		m_batchBuffer->SetData(m_batchDataArr, sizeof(BatchedVertexData) * m_batchDataArrIndex);
		m_batchIndices->SetData(m_batchIndicesArr, m_batchIndicesArrIndex);

		m_batchBuffer->Bind();
		RenderCommand::DrawIndexed(m_batchIndices);
	}

	void DrawDataBuffer::ClearBatching()
	{
		m_batchDataArrIndex = 0;
		m_batchIndicesArrIndex = 0;
		m_batchTextureSlotIndex = 0;
	}

	void DrawDataBuffer::BatchRenderMeshSection(BatchedVertexData* vertexData, size_t numVertex,
		const ADynArr<unsigned int>& indices, size_t dataOffset, size_t drawCallSize)
	{
		unsigned int* indexArr = new unsigned int[drawCallSize];
		BatchedVertexData* vertexDataArr = new BatchedVertexData[drawCallSize];

		for (size_t i = 0; i < drawCallSize; i++)
		{
			unsigned int currIndex = indices[i + dataOffset];
			AE_CORE_ASSERT(currIndex < numVertex, "Index out of bounds");
			vertexDataArr[i] = vertexData[currIndex];
			indexArr[i] = i;
		}

		BindTextures(m_batchTextureSlots, m_batchTextureSlotIndex);

		m_batchBuffer->Bind();
		m_batchBuffer->SetData(vertexDataArr, sizeof(BatchedVertexData) * drawCallSize);

		m_instancingIndices->Bind();
		m_instancingIndices->SetData(indexArr, drawCallSize);
		RenderCommand::DrawIndexed(m_instancingIndices);

		delete[] indexArr;
		delete[] vertexDataArr;
	}

	void DrawDataBuffer::CollectMeshesToInstance(ASinglyLinkedList<MeshHandle>& toInstance)
	{
		for (auto& pair : m_meshUseCounts)
		{
			if (pair.GetElement() >= s_instancingCutoff)
			{
				toInstance.Add(pair.GetKey());
			}
		}
	}

	void DrawDataBuffer::RenderInstancing(const Mat4& viewProj, AUnorderedMap<MeshHandle,
		ASinglyLinkedList<DrawCommand*>> commandsToInstance)
	{
		for (auto& pair : commandsToInstance)
		{
			RenderMeshInstance(viewProj, pair.GetKey(), pair.GetElement());
		}
	}

	void DrawDataBuffer::RenderMeshInstance(const Mat4& viewProj, MeshHandle mesh, 
		ASinglyLinkedList<DrawCommand*>& commands)
	{
		AReference<Mesh> meshToInstance = ResourceHandler::GetMesh(mesh);

		AE_CORE_ASSERT(meshToInstance != nullptr, "");

		size_t numVertices = meshToInstance->GetPositions().GetCount();
		VertexData* vertexDataArr = new VertexData[numVertices];
		ReadVertexDataFromMesh(meshToInstance, vertexDataArr, 0, numVertices);
		
		const ADynArr<unsigned int>& indices = meshToInstance->GetIndices();

		InstanceVertexData* instanceData = new InstanceVertexData[commands.GetCount()];
		size_t index = 0;

		for (DrawCommand* cmd : commands)
		{
			instanceData[index].transform = cmd->GetTransform();
			instanceData[index].color = cmd->GetColor();
			instanceData[index].textureIndex = (float)GetTextureIndex(m_instancingTextureSlots, 
				m_instancingTextureSlotIndex, cmd->GetTexture());
			index++;
		}

		//if (numVertices < s_maxNumVertex)
		if (false)
		{
			BindTextures(m_instancingTextureSlots, m_instancingTextureSlotIndex);

			m_instancingBuffer->Bind();
			m_instancingBuffer->SetData(vertexDataArr, sizeof(VertexData) * numVertices);

			m_instancingIndices->Bind();
			m_instancingIndices->SetData(indices.GetData(), indices.GetCount());

			m_instancingArr->Bind();
			m_instancingArr->SetData(instanceData, sizeof(InstanceVertexData) * commands.GetCount());

			m_instancingBuffer->Bind();
			m_instancingIndices->Bind();
			RenderCommand::DrawInstancedIndexed(m_instancingIndices, commands.GetCount());
		}
		else
		{
			testing 2 textures for all 4 branches
			currently only the first texture is being renderer in the currently selected branch (find why)

			size_t drawCallSize = ComputeDrawCallSize();
			size_t offset = 0;

			while (offset < indices.GetCount())
			{
				size_t currDrawSize = Math::Min(drawCallSize, indices.GetCount() - offset);
				InstanceRenderMeshSection(vertexDataArr, numVertices, indices, instanceData,
					commands.GetCount(), offset, currDrawSize);
				offset += currDrawSize;
			}
		}
		delete[] instanceData;
	}

	void DrawDataBuffer::InstanceRenderMeshSection(VertexData* vertexData, size_t numVertex,
		const ADynArr<unsigned int>& indices, InstanceVertexData* instanceData,
		size_t numInstanceData, size_t dataOffset, size_t drawCallSize)
	{
		AE_CORE_ASSERT(drawCallSize > 0, "Invalid draw call size");
		unsigned int* indexArr = new unsigned int[drawCallSize];
		VertexData* vertexDataArr = new VertexData[drawCallSize];

		for (size_t i = 0; i < drawCallSize; i++)
		{
			unsigned int currIndex = indices[i + dataOffset];
			AE_CORE_ASSERT(currIndex < numVertex, "Index out of bounds");
			vertexDataArr[i] = vertexData[currIndex];
			indexArr[i] = i;
		}

		BindTextures(m_instancingTextureSlots, m_instancingTextureSlotIndex);

		m_instancingBuffer->Bind();
		m_instancingBuffer->SetData(vertexDataArr, sizeof(VertexData) * drawCallSize);

		m_instancingIndices->Bind();
		m_instancingIndices->SetData(indexArr, drawCallSize);

		m_instancingArr->Bind();
		m_instancingArr->SetData(instanceData, sizeof(InstanceVertexData) * numInstanceData);

		m_instancingBuffer->Bind();
		m_instancingIndices->Bind();
		RenderCommand::DrawInstancedIndexed(m_instancingIndices, numInstanceData);

		delete[] indexArr;
		delete[] vertexDataArr;
	}


	
	// RenderingDataSorter ////////////////////////////////////////////////////

	void RenderingDataSorter::AddData(DrawCommand* data)
	{
		AE_CORE_ASSERT(data != nullptr, "");
		MaterialHandle mat = data->GetMaterial();
		if (!m_buffers.ContainsKey(mat))
		{
			m_buffers.Add(mat, DrawDataBuffer());
			m_buffers[mat].Initialize();
		}
		m_buffers[data->GetMaterial()].AddDrawCommand(data);
	}

	void RenderingDataSorter::Clear()
	{
		for (auto& pair : m_buffers)
		{
			pair.GetElement().Clear();
		}
	}

	RenderingDataSorter::AIterator RenderingDataSorter::begin()
	{
		return m_buffers.begin();
	}

	RenderingDataSorter::AIterator RenderingDataSorter::end()
	{
		return m_buffers.end();
	}

	RenderingDataSorter::AConstIterator RenderingDataSorter::begin() const
	{
		return m_buffers.begin();
	}

	RenderingDataSorter::AConstIterator RenderingDataSorter::end() const
	{
		return m_buffers.end();
	}

}