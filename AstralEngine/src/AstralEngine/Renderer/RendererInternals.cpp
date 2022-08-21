#include "aepch.h"
#include "RendererInternals.h"
#include "Mesh.h"

namespace AstralEngine
{
	void DrawDataBuffer::TempRenderFunc(MeshHandle mesh)
	{
		DrawCommand cmd = DrawCommand(Mat4::Identity(), NullHandle, mesh, Vector4(1, 1, 1, 1), NullEntity);
		ASinglyLinkedList<DrawCommand*> l;
		l.Add(&cmd);

		auto shader = ResourceHandler::GetShader(0);
		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", Mat4::Identity());

		RenderMeshInstance(Mat4::Identity(), mesh, l);
	}

	struct InstanceVertexData
	{
		Mat4 transform;
		Vector4 color;
	};

	// DrawCommand /////////////////////////////////
	
	DrawCommand::DrawCommand() { }
	DrawCommand::DrawCommand(const Mat4& transform, MaterialHandle mat, MeshHandle mesh, 
		const Vector4& color, const AEntity e) : m_transform(transform), m_mesh(mesh), 
		m_material(mat), m_color(color), m_entity(e) { }

	const Mat4& DrawCommand::GetTransform() const { return m_transform; }
	MaterialHandle DrawCommand::GetMaterial() const { return m_material; }
	MeshHandle DrawCommand::GetMesh() const { return m_mesh; }
	const Vector4& DrawCommand::GetColor() const { return m_color; }
	AEntity DrawCommand::GetEntity() const { return m_entity; }
	bool DrawCommand::IsOpaque() const { return ResourceHandler::GetMaterial(m_material)->GetColor().a == 1.0f; }

	bool DrawCommand::operator==(const DrawCommand& other) const
	{
		return m_mesh == other.m_mesh && m_material == other.m_material
			&& m_transform == other.m_transform;
	}

	bool DrawCommand::operator!=(const DrawCommand& other) const
	{
		return !(*this == other);
	}

	// DrawDataBuffer ////////////////////////////////////////////////////

	size_t DrawDataBuffer::s_maxNumVertex = 0;
	size_t DrawDataBuffer::s_maxNumIndices;


	DrawDataBuffer::DrawDataBuffer()
	{
		// temp ////////////////////
		m_batchDataArr = nullptr;
		m_batchIndicesArr = nullptr;
		////////////////////////////
	}

	DrawDataBuffer::~DrawDataBuffer()
	{
		delete[] m_batchDataArr;
		delete[] m_batchIndicesArr;
	}

	void DrawDataBuffer::Initialize()
	{
		if (s_maxNumVertex == 0)
		{
			s_maxNumVertex = RenderCommand::GetMaxNumVertices();
			s_maxNumIndices = RenderCommand::GetMaxNumIndices();
		}

		m_instancingBuffer = VertexBuffer::Create(s_maxNumVertex * sizeof(VertexData));
		m_instancingArr = VertexBuffer::Create(s_maxNumVertex * sizeof(InstanceVertexData), true);
		m_instancingIndices = IndexBuffer::Create();

		m_instancingBuffer->Bind();
		m_instancingBuffer->SetLayout({
			{ ADataType::Float3, "position" }
			});
		
		m_instancingArr->Bind();
		m_instancingArr->SetLayout({
			{ ADataType::Mat4, "transform", false, 1 },
			{ ADataType::Float4, "color", false, 1 },
			}, 1);
	}

	void DrawDataBuffer::Draw(const Mat4& viewProj, MaterialHandle material)
	{
		ASinglyLinkedList<MeshHandle> meshesToInstance;
		AUnorderedMap<MeshHandle, ASinglyLinkedList<DrawCommand*>> commandsToInstance;
		CollectMeshesToInstance(meshesToInstance);

		for (DrawCommand* cmd : m_drawCommands)
		{
			if (meshesToInstance.Contains(cmd->GetMesh()))
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
				AddToBatching(cmd);
			}
		}

		AReference<Material> mat = ResourceHandler::GetMaterial(material);
		AE_CORE_ASSERT(mat != nullptr, "");

		//AReference<Shader> shader = ResourceHandler::GetShader(mat->GetShader());
		AReference<Shader> shader = ResourceHandler::GetShader(0); // temp
		AE_CORE_ASSERT(shader != nullptr, "");

		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", viewProj);

		RenderBatch(viewProj);
		RenderInstancing(viewProj, commandsToInstance);
	}

	void DrawDataBuffer::AddDrawCommand(DrawCommand* draw)
	{
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
		m_batchDataArrCount = 0;
		m_batchIndicesArrCount = 0;
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

	void DrawDataBuffer::AddToBatching(DrawCommand* cmd)
	{
		AE_CORE_ERROR("Not yet implemented");
	}
	
	void DrawDataBuffer::RenderBatch(const Mat4& viewProj)
	{

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
			index++;
		}

		if (numVertices < s_maxNumVertex)
		//if (false)
		{
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
			AE_CORE_ERROR("not ready yet");
			size_t drawCallSize;
			{
				// make sure numVertex is a multiple of 3 since we are drawing triangles
				size_t maxVertexSize = s_maxNumVertex - (s_maxNumVertex % 3);
				size_t maxIndexSize = s_maxNumIndices - (s_maxNumIndices % 3);

				drawCallSize = Math::Min(maxVertexSize, maxIndexSize);
				drawCallSize = 3; //temp
			}

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
	
	void DrawDataBuffer::ReadVertexDataFromMesh(AReference<Mesh>& mesh, VertexData* vertexDataArr, 
		size_t dataOffset, size_t dataCount)
	{
		const ADynArr<Vector3>& positions = mesh->GetPositions();
		for (size_t i = 0; i < positions.GetCount(); i++)
		{
			vertexDataArr[dataOffset + i].position = positions[i];
		}
	}




	// DrawCallList /////////////////////////////////////////////////
	DrawCallList::DrawCallList() : m_material(NullHandle) { }
	DrawCallList::DrawCallList(MaterialHandle material, MeshHandle mesh) : m_material(material), m_mesh(mesh)
	{
		constexpr size_t vbSize = 400;
		m_geometryDataBuffer = VertexBuffer::Create(vbSize);
		m_instanceArrBuffer = VertexBuffer::Create(vbSize, true);
		m_indexBuffer = IndexBuffer::Create();
		SetupGeometryData();
	}
	
	DrawCallList::~DrawCallList()
	{
		Clear();
	}

	MaterialHandle DrawCallList::GetMaterial() const { return m_material; }

	MeshHandle DrawCallList::GetMesh() const { return m_mesh; }

	void DrawCallList::Draw(const Mat4& viewProj) const
	{
		if (m_material != NullHandle)
		{
			AReference<Material> mat = ResourceHandler::GetMaterial(m_material);
			if (mat != nullptr)
			{
				//AReference<Shader> shader = ResourceHandler::GetShader(mat->GetShader());
				AReference<Shader> shader = ResourceHandler::GetShader(0); // temp
				shader->Bind();
				shader->SetMat4("u_viewProjMatrix", viewProj);

				InstanceVertexData* vertexData = new InstanceVertexData[m_data.GetCount()];
				size_t index = 0;
				for (DrawCommand* cmd : m_data)
				{
					//vertexData[index].transform = cmd->GetTransform();
					//vertexData[index].color = cmd->GetColor();
					index++;
				}

				m_geometryDataBuffer->Bind();
				m_instanceArrBuffer->Bind();
				m_instanceArrBuffer->SetData(vertexData, sizeof(InstanceVertexData) * m_data.GetCount());
				delete[] vertexData;

				m_geometryDataBuffer->Bind();
				m_indexBuffer->Bind();
				RenderCommand::DrawInstancedIndexed(m_indexBuffer, m_data.GetCount());
			}
		}
	}

	void DrawCallList::AddDrawCommand(DrawCommand* draw)
	{
		AE_CORE_ASSERT(draw->GetMaterial() == m_material && draw->GetMesh() == m_mesh, "");
		m_data.Add(draw);
	}

	void DrawCallList::Clear()
	{
		for (DrawCommand* cmd : m_data)
		{
			delete cmd;
		}
		m_data.Clear();
	}

	void DrawCallList::SetupGeometryData()
	{
		SetupQuad();
	}

	void DrawCallList::SetupQuad()
	{
		Vector3 pos[] =
		{
			{ -0.5f, -0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f },
			{  0.5f,  0.5f, 0.0f },
			{ -0.5f,  0.5f, 0.0f }
		};

		m_geometryDataBuffer->Bind();
		m_geometryDataBuffer->SetLayout({ { ADataType::Float3, "position" } });
		m_geometryDataBuffer->SetData(pos, sizeof(pos));

		m_instanceArrBuffer->Bind();
		m_instanceArrBuffer->SetLayout({ { ADataType::Mat4, "transformMatrix", false, 1 }, 
			{ ADataType::Float4, "color", false, 1 } }, 1);

		unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		m_indexBuffer->SetData(indices, sizeof(indices) / sizeof(unsigned int));
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