#include "aepch.h"
#include "RendererInternals.h"
#include "AstralEngine/Core/Application.h"
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
		const AEntity e, bool opaque, Texture2DHandle texture) : m_transform(transform), m_mesh(mesh), 
		m_material(mat), m_color(color), m_entity(e), m_texture(texture), m_opaque(opaque)
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

	bool DrawCommand::IsOpaque() const { return m_opaque; }

	bool DrawCommand::UsesDeferred() const
	{
		return ResourceHandler::GetMaterial(m_material)->UsesDeferredRendering();
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

	// GBuffer ////////////////////////////////////////////////////////////////

	GBuffer::GBuffer()
	{
		m_framebuffer = Framebuffer::Create(s_framebufferWidth, s_framebufferHeight);
		m_framebuffer->Bind();
		m_framebuffer->SetColorAttachment(ResourceHandler::CreateTexture2D(s_framebufferWidth, s_framebufferHeight), 1);
		m_framebuffer->SetColorAttachment(ResourceHandler::CreateTexture2D(s_framebufferWidth, s_framebufferHeight), 2);
		m_framebuffer->Unbind();
	}

	void GBuffer::Bind()
	{
		m_framebuffer->Bind();
		RenderCommand::SetViewport(0, 0, s_framebufferWidth, s_framebufferHeight);
		RenderCommand::Clear();
	}

	void GBuffer::Unbind()
	{
		m_framebuffer->Unbind();
		AWindow* window = Application::GetWindow();
		RenderCommand::SetViewport(0, 0, window->GetWidth(), window->GetHeight());
	}

	AReference<Shader> GBuffer::PrepareForRender(const Mat4& viewProjMatrix)
	{
		AReference<Material> mat = ResourceHandler::GetMaterial(Material::GBufferMat());
		AReference<Shader> shader = ResourceHandler::GetShader(mat->GetShader());

		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", viewProjMatrix);
		return shader;
	}

	void GBuffer::BindTexureData()
	{
		AReference<Texture2D> t1 = ResourceHandler::GetTexture2D(m_framebuffer->GetColorAttachment());
		AReference<Texture2D> t2 = ResourceHandler::GetTexture2D(m_framebuffer->GetColorAttachment(1));
		AReference<Texture2D> t3 = ResourceHandler::GetTexture2D(m_framebuffer->GetColorAttachment(2));

		t1->Bind();
		t2->Bind(1);
		t3->Bind(2);
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
		m_batchBuffer = VertexBuffer::Create(s_maxNumVertex * sizeof(VertexData));
		m_batchIndices = IndexBuffer::Create();

		m_batchBuffer->Bind();
		m_batchBuffer->SetLayout({ 
			{ ADataType::Float3, "position" },
			{ ADataType::Float3, "normal" },
			{ ADataType::Float2, "textureCoords" },
			{ ADataType::Mat4, "transform" },
			{ ADataType::Float4, "color" },
			{ ADataType::Float, "textureIndex" }
			});

		m_batchDataArr = new BatchedVertexData[s_maxNumVertex];
		m_batchDataArrIndex = 0;

		m_batchIndicesArr = new unsigned int[s_maxNumIndices];
		m_batchIndicesArrIndex = 0;

		m_batchTextureSlots = new Texture2DHandle[s_numTextureSlots];
		m_batchTextureSlotIndex = 0;
		m_hasBatchedData = false;

		// Instancing
		m_instancingBuffer = VertexBuffer::Create(s_maxNumVertex * sizeof(VertexData));
		m_instancingArr = VertexBuffer::Create(s_maxNumVertex * sizeof(InstanceVertexData), true);
		m_instancingIndices = IndexBuffer::Create();

		m_instancingBuffer->Bind();
		m_instancingBuffer->SetLayout({
			{ ADataType::Float3, "position" },
			{ ADataType::Float3, "normal" },
			{ ADataType::Float2, "textureCoords" }
			});
		

		m_instancingArr->Bind();
		m_instancingArr->SetLayout({
			{ ADataType::Mat4, "transform", false, 1 },
			{ ADataType::Float4, "color", false, 1 },
			{ ADataType::Float, "textureIndex", false, 1 }
			}, 3);

		m_instancingTextureSlots = new Texture2DHandle[s_numTextureSlots];
		m_instancingTextureSlotIndex = 0;
	}

	void DrawDataBuffer::Draw(const Mat4& viewProj, MaterialHandle material)
	{
		if (IsEmpty())
		{
			return;
		}

		AReference<Material> mat = ResourceHandler::GetMaterial(material);
		AE_RENDER_ASSERT(mat != nullptr, "");

		AReference<Shader> shader = ResourceHandler::GetShader(mat->GetShader());
		AE_RENDER_ASSERT(shader != nullptr, "");

		shader->Bind();
		shader->SetMat4("u_viewProjMatrix", viewProj);
		mat->SendUniformsToShader();

		if (mat->UsesDeferredRendering())
		{
			Renderer::BindGBufferTextures();
		}

		RenderGeometry(viewProj);
	}

	void DrawDataBuffer::RenderGeometry(const Mat4& viewProj)
	{
		if (IsEmpty())
		{
			return;
		}

		for (auto& meshCommandPair : m_commandsToBatch)
		{
			for (DrawCommand* cmd : meshCommandPair.GetElement())
			{
				AddToBatching(viewProj, cmd);
			}
		}

		RenderBatch(viewProj);
		RenderInstancing(viewProj, m_commandsToInstance);
	}

	void DrawDataBuffer::AddDrawCommand(DrawCommand* draw)
	{
		AE_RENDER_ASSERT(draw->GetMesh() != NullHandle, "");
		m_drawCommands.Add(draw);
		if (m_meshUseCounts.ContainsKey(draw->GetMesh()))
		{
			m_meshUseCounts[draw->GetMesh()]++;
		}
		else
		{
			m_meshUseCounts.Add(draw->GetMesh(), 1);
		}

		size_t count = m_meshUseCounts[draw->GetMesh()];
		if (count > s_instancingCutoff)
		{
			m_commandsToInstance[draw->GetMesh()].Add(draw);
		}
		else if (count == s_instancingCutoff)
		{
			m_commandsToInstance.Add(draw->GetMesh(), m_commandsToBatch[draw->GetMesh()]);
			m_commandsToInstance[draw->GetMesh()].Add(draw);
			m_commandsToBatch.Remove(draw->GetMesh());
		}
		else
		{
			m_commandsToBatch[draw->GetMesh()].Add(draw);
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
		m_commandsToBatch.Clear();
		m_commandsToInstance.Clear();
		ClearBatching();
		ClearInstancing();
	}

	bool DrawDataBuffer::IsEmpty() const { return m_drawCommands.IsEmpty(); }

	void DrawDataBuffer::ReadVertexDataFromMesh(AReference<Mesh>& mesh, VertexData* vertexDataArr,
		size_t dataOffset, size_t dataCount)
	{
		const ADynArr<Vector3>& positions = mesh->GetPositions();
		const ADynArr<Vector3>& normals = mesh->GetNormals();
		const ADynArr<Vector2>& textureCoords = mesh->GetTextureCoords();
		for (size_t i = 0; i < positions.GetCount(); i++)
		{
			vertexDataArr[dataOffset + i].position = positions[i];
			vertexDataArr[dataOffset + i].textureCoords = textureCoords[i];
			vertexDataArr[dataOffset + i].normal = normals[i];
		}
	}

	int DrawDataBuffer::GetTextureIndex(Texture2DHandle* arr, size_t& index, Texture2DHandle texture)
	{
		if (texture == NullHandle)
		{
			return -2;
		}

		for (size_t i = 0; i < index; i++)
		{
			if (arr[i] == texture)
			{
				return (int)i;
			}
		}

		if (index >= s_numTextureSlots)
		{
			return -1;
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
			AE_RENDER_ASSERT(texture != nullptr, "");
			texture->Bind((unsigned int)i);
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
		AE_RENDER_ASSERT(mesh != nullptr, "");
		const ADynArr<Vector3>& positions = mesh->GetPositions();
		const ADynArr<Vector3>& normals = mesh->GetNormals();
		const ADynArr<Vector2>& textureCoords = mesh->GetTextureCoords();
		const ADynArr<unsigned int>& indices = mesh->GetIndices();
		int textureIndex = GetTextureIndex(m_batchTextureSlots, m_batchTextureSlotIndex, cmd->GetTexture());

		if (textureIndex == -1)
		{
			RenderBatch(viewProj);
			ClearBatching();
			textureIndex = GetTextureIndex(m_batchTextureSlots, m_batchTextureSlotIndex, cmd->GetTexture());
		}

		if (positions.GetCount() >= s_maxNumVertex || indices.GetCount() >= s_maxNumIndices)
		//if (true)
		{
			// split up mesh for render here
			size_t numVertices = positions.GetCount();
			BatchedVertexData* vertexDataArr = new BatchedVertexData[numVertices];


			for (size_t i = 0; i < numVertices; i++)
			{
				vertexDataArr[i].vertex.position = positions[i];
				vertexDataArr[i].vertex.normal = normals[i];
				vertexDataArr[i].vertex.textureCoords = textureCoords[i];
				vertexDataArr[i].instance.color = cmd->GetColor();
				vertexDataArr[i].instance.transform = cmd->GetTransform();
				vertexDataArr[i].instance.textureIndex = (float)textureIndex;
			}

			size_t drawCallSize = ComputeDrawCallSize();
			//drawCallSize = 3; // temp
			size_t offset = 0;
			size_t currDrawSize = indices.GetCount();
			while (offset < indices.GetCount())
			{
				RenderBatch(viewProj);
				ClearBatching();

				currDrawSize = Math::Min(drawCallSize, indices.GetCount() - offset);
				BatchRenderMeshSection(vertexDataArr, numVertices, indices, offset, currDrawSize);
				offset += currDrawSize;
				m_hasBatchedData = true;
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
			m_batchDataArr[m_batchDataArrIndex + i].vertex.normal = normals[i];
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
		m_hasBatchedData = true;
	}

	void DrawDataBuffer::RenderBatch(const Mat4& viewProj)
	{
		if (m_hasBatchedData)
		{
			BindTextures(m_batchTextureSlots, m_batchTextureSlotIndex);

			m_batchBuffer->SetData(m_batchDataArr, sizeof(BatchedVertexData) * m_batchDataArrIndex);
			m_batchIndices->SetData(m_batchIndicesArr, m_batchIndicesArrIndex);

			m_batchBuffer->Bind();
			RenderCommand::DrawIndexed(m_batchIndices);

			// update stats
			Renderer::s_stats.numIndices += m_batchIndicesArrIndex;
			Renderer::s_stats.numVertices += m_batchDataArrIndex;
			Renderer::s_stats.numDrawCalls++;
		}
	}

	void DrawDataBuffer::ClearBatching()
	{
		m_batchDataArrIndex = 0;
		m_batchIndicesArrIndex = 0;
		m_batchTextureSlotIndex = 0;
		m_hasBatchedData = false;
	}

	void DrawDataBuffer::BatchRenderMeshSection(BatchedVertexData* vertexData, size_t numVertex,
		const ADynArr<unsigned int>& indices, size_t dataOffset, size_t drawCallSize)
	{
		

		unsigned int* indexArr = new unsigned int[drawCallSize];
		BatchedVertexData* vertexDataArr = new BatchedVertexData[drawCallSize];

		for (size_t i = 0; i < drawCallSize; i++)
		{
			unsigned int currIndex = indices[i + dataOffset];
			AE_RENDER_ASSERT(currIndex < numVertex, "Index out of bounds");
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

		// update stats
		Renderer::s_stats.numIndices += drawCallSize;
		Renderer::s_stats.numVertices += drawCallSize;
		Renderer::s_stats.numDrawCalls++;
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
		ADynArr<DrawCommand*>>& commandsToInstance)
	{
		

		for (auto& pair : commandsToInstance)
		{
			RenderMeshInstance(viewProj, pair.GetKey(), pair.GetElement());
		}
	}

	void DrawDataBuffer::RenderMeshInstance(const Mat4& viewProj, MeshHandle mesh, 
		ADynArr<DrawCommand*>& commands)
	{
		

		if (commands.IsEmpty())
		{
			return;
		}

		AReference<Mesh> meshToInstance = ResourceHandler::GetMesh(mesh);

		AE_RENDER_ASSERT(meshToInstance != nullptr, "");

		size_t numVertices = meshToInstance->GetPositions().GetCount();
		VertexData* vertexDataArr = new VertexData[numVertices];
		ReadVertexDataFromMesh(meshToInstance, vertexDataArr, 0, numVertices);
		
		const ADynArr<unsigned int>& indices = meshToInstance->GetIndices();

		InstanceVertexData* instanceData = new InstanceVertexData[commands.GetCount()];
		size_t index = 0;
		size_t indexOffset = 0;
		auto it = commands.begin();

		while (index < commands.GetCount())
		{
			int textureIndex;

			for (; it != commands.end(); it++)
			{
				DrawCommand* cmd = *it;
				textureIndex = (float)GetTextureIndex(m_instancingTextureSlots,
					m_instancingTextureSlotIndex, cmd->GetTexture());

				if (textureIndex == -1)
				{
					// no more texture slots
					break;
				}

				instanceData[index].transform = cmd->GetTransform();
				instanceData[index].color = cmd->GetColor();
				instanceData[index].textureIndex = (float)textureIndex;
				index++;
			}

			if (numVertices < s_maxNumVertex)
			//if (false)
			{
				size_t numInstancesToRender = index - indexOffset;
				BindTextures(m_instancingTextureSlots, m_instancingTextureSlotIndex);

				m_instancingBuffer->Bind();
				m_instancingBuffer->SetData(vertexDataArr, sizeof(VertexData) * numVertices);

				m_instancingIndices->Bind();
				m_instancingIndices->SetData(indices.GetData(), indices.GetCount());

				m_instancingArr->Bind();
				m_instancingArr->SetData(&instanceData[indexOffset], 
					sizeof(InstanceVertexData) * numInstancesToRender);

				m_instancingBuffer->Bind();
				m_instancingIndices->Bind();
				RenderCommand::DrawInstancedIndexed(m_instancingIndices, numInstancesToRender);

				// update stats
				Renderer::s_stats.numIndices += indices.GetCount() * numInstancesToRender;
				Renderer::s_stats.numVertices += numVertices * numInstancesToRender;
				Renderer::s_stats.numDrawCalls++;				
			}
			else
			{
				size_t drawCallSize = ComputeDrawCallSize();
				drawCallSize = 3; // temp
				size_t offset = 0;

				while (offset < indices.GetCount())
				{
					size_t currDrawSize = Math::Min(drawCallSize, indices.GetCount() - offset);
					InstanceRenderMeshSection(vertexDataArr, numVertices, indices, &instanceData[indexOffset],
						index - indexOffset, offset, currDrawSize);
					offset += currDrawSize;
				}
			}

			if (textureIndex == -1)
			{
				indexOffset = index;
				ClearInstancing();
			}
		}
		delete[] instanceData;
	}

	void DrawDataBuffer::InstanceRenderMeshSection(VertexData* vertexData, size_t numVertex,
		const ADynArr<unsigned int>& indices, InstanceVertexData* instanceData,
		size_t numInstanceData, size_t dataOffset, size_t drawCallSize)
	{
		

		AE_RENDER_ASSERT(drawCallSize > 0, "Invalid draw call size");
		unsigned int* indexArr = new unsigned int[drawCallSize];
		VertexData* vertexDataArr = new VertexData[drawCallSize];

		for (size_t i = 0; i < drawCallSize; i++)
		{
			unsigned int currIndex = indices[i + dataOffset];
			AE_RENDER_ASSERT(currIndex < numVertex, "Index out of bounds");
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

		// update stats
		Renderer::s_stats.numIndices += drawCallSize;
		Renderer::s_stats.numVertices += drawCallSize;
		Renderer::s_stats.numDrawCalls++;
	}

	void DrawDataBuffer::ClearInstancing()
	{
		m_instancingTextureSlotIndex = 0;
	}

	
	// RenderingDataSorter ////////////////////////////////////////////////////

	void RenderingDataSorter::AddData(DrawCommand* data)
	{
		AE_RENDER_ASSERT(data != nullptr, "");
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

	bool RenderingDataSorter::IsEmpty() const
	{
		if (m_buffers.GetCount() == 0)
		{
			return true;
		}

		for (auto& pair : m_buffers)
		{
			if (!pair.GetElement().IsEmpty())
			{
				return false;
			}
		}
		return true;
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

	// RenderQueue /////////////////////////////////////////////////////////////////////
	RenderQueue::RenderQueue(GBuffer* gBuffer) : m_gBuffer(gBuffer), m_transparent(nullptr)
	{
		if (gBuffer == nullptr)
		{
			m_transparent = new RenderingDataSorter();
		}
		else
		{
			SetupFullscreenRenderingObjects();
		}
	}

	RenderQueue::~RenderQueue() { }

	void RenderQueue::AddData(DrawCommand* data)
	{
		AE_CORE_ASSERT(data->UsesDeferred() == (m_gBuffer != nullptr), 
			"Draw Command passed to the wrong render queue");

		if (data->UsesDeferred())
		{
			AE_CORE_ASSERT(m_gBuffer != nullptr, "Draw Command passed to the wrong render queue");
			AE_CORE_ASSERT(data->IsOpaque(), "Deferred render queue does not support transparent material");
			m_opaque.AddData(data);
		}
		else 
		{
			if (data->IsOpaque())
			{
				m_opaque.AddData(data);
			}
			else
			{
				m_transparent->AddData(data);
			}
		}
	}

	void RenderQueue::Draw(const Mat4& viewProj)
	{
		if (m_gBuffer != nullptr)
		{
			// deferred rendering
			m_gBuffer->Bind();
			RenderCommand::EnableBlending(false);
			AReference<Shader> shader = m_gBuffer->PrepareForRender(viewProj);

			for (auto& pair : m_opaque)
			{
				AReference<Material> currMat = ResourceHandler::GetMaterial(pair.GetKey());
				AE_RENDER_ASSERT(currMat != nullptr, "");

				Texture2DHandle diffuseMap = currMat->GetDiffuseMap();
				Texture2DHandle specularMap = currMat->GetSpecularMap();
				if (diffuseMap == NullHandle)
				{
					diffuseMap = Texture2D::WhiteTexture();
				}

				if (specularMap == NullHandle)
				{
					specularMap = Texture2D::WhiteTexture();
				}

				Vector4 color = currMat->GetColor();
				if (!currMat->HasColor())
				{
					color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				}

				shader->SetFloat4("u_matColor", color);

				ResourceHandler::GetTexture2D(diffuseMap)->Bind();
				ResourceHandler::GetTexture2D(specularMap)->Bind(1);

				pair.GetElement().RenderGeometry(viewProj);
			}
			m_gBuffer->Unbind();
			RenderCommand::EnableBlending(true);
			RenderCommand::Clear();

			need to copy gbuffer fb to default fb so we can use forward rendering with the deferred

			shader = ResourceHandler::GetShader(m_deferredShader);
			shader->Bind();
			SendLightUniformsToShader(shader);
			shader->SetFloat3("u_camPos", Renderer::GetCamPos());
			m_gBuffer->BindTexureData();

			m_deferredVB->Bind();
			m_deferredIB->Bind();
			RenderCommand::DrawIndexed(m_deferredIB);
			/*
			*/

			/*
			shader = ResourceHandler::GetShader(Shader::FullscreenQuadShader());
			shader->Bind();
			shader->SetInt("u_texture", 0);
			static Texture2DHandle pos = 0;
			static Texture2DHandle normal = 2;
			static Texture2DHandle color = 3;
			ResourceHandler::GetTexture2D(normal)->Bind();
			m_deferredVB->Bind();
			m_deferredIB->Bind();
			RenderCommand::DrawIndexed(m_deferredIB);
			*/

		}
		else
		{
			// forward rendering
			for (auto& pair : m_opaque)
			{
				pair.GetElement().Draw(viewProj, pair.GetKey());
			}

			for (auto& pair : *m_transparent)
			{
				pair.GetElement().Draw(viewProj, pair.GetKey());
			}
		}
	}

	void RenderQueue::Clear()
	{
		m_opaque.Clear();
		if (m_transparent != nullptr)
		{
			m_transparent->Clear();
		}
	}

	void RenderQueue::BindGBufferTextureData() { m_gBuffer->BindTexureData(); }

	void RenderQueue::SendLightUniformsToShader(AReference<Shader>& shader)
	{
		if (!Renderer::LightsModified() || shader == nullptr)
		{
			return;
		}
		const ADynArr<LightData>& lightData = Renderer::GetLightData();

		shader->SetFloat3("u_lightPos", lightData[0].GetPosition());
		shader->SetFloat3("u_lightAmbient", lightData[0].GetAmbientColor());
		shader->SetFloat3("u_lightDiffuse", lightData[0].GetDiffuseColor());
		shader->SetFloat3("u_lightSpecular", lightData[0].GetSpecularColor());
	}

	void RenderQueue::SetupFullscreenRenderingObjects()
	{
		m_deferredShader = Shader::DefaultShader();

		{
			AReference<Shader> shader = ResourceHandler::GetShader(m_deferredShader);
			shader->Bind();
			shader->SetInt("u_positionGBuffer", 0);
			shader->SetInt("u_normalGBuffer", 1);
			shader->SetInt("u_colorGBuffer", 2);
		}

		constexpr float data[] =
		{
			// position   +   texture coords
			-1.0f, -1.0f,     0.0f, 0.0f,
			 1.0f, -1.0f,     1.0f, 0.0f,
			 1.0f,  1.0f,     1.0f, 1.0f,
			-1.0f,  1.0f,     0.0f, 1.0f
		};

		constexpr unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		m_deferredVB = VertexBuffer::Create((float*)data, sizeof(data));
		m_deferredVB->Bind();
		m_deferredVB->SetLayout({ 
			{ ADataType::Float2, "position" },
			{ ADataType::Float2, "textureCoords" }
			});

		m_deferredIB = IndexBuffer::Create((unsigned int*)indices, sizeof(indices) / sizeof(unsigned int));

	}

}