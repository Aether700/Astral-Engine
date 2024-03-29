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
		AWindow* window = Application::GetWindow();
		unsigned int width = window->GetWidth();
		unsigned int height = window->GetHeight();
		m_framebuffer = Framebuffer::Create(width, height);
		m_framebuffer->Bind();
		m_framebuffer->SetColorAttachment(ResourceHandler::CreateTexture2D(width, height,
			Texture2DInternalFormat::RGB16Normal), 0);
		m_framebuffer->SetColorAttachment(ResourceHandler::CreateTexture2D(width, height, 
			Texture2DInternalFormat::RGB16Normal), 1);
		m_framebuffer->SetColorAttachment(ResourceHandler::CreateTexture2D(width, height), 2);
		m_framebuffer->Unbind();
	}

	void GBuffer::Bind()
	{
		m_framebuffer->Bind();
		RenderCommand::SetViewport(0, 0, m_framebuffer->GetWidth(), m_framebuffer->GetHeight());
		RenderCommand::Clear();
	}

	void GBuffer::Unbind()
	{
		m_framebuffer->Unbind();
		AWindow* window = Application::GetWindow();
		RenderCommand::SetViewport(0, 0, window->GetWidth(), window->GetHeight());
	}

	void GBuffer::OnWindowResize(WindowResizeEvent& resize)
	{
		m_framebuffer->Resize(resize.GetWidth(), resize.GetHeight());
	}

	const AReference<Framebuffer>& GBuffer::GetFramebuffer() const { return m_framebuffer; }

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

	void RenderQueue::OnWindowResize(WindowResizeEvent& resize)
	{
		if (m_gBuffer != nullptr)
		{
			m_gBuffer->OnWindowResize(resize);
		}
	}

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
			Vector4 clearColor = RenderCommand::GetClearColor();
			// deferred rendering
			m_gBuffer->Bind();
			RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			RenderCommand::Clear();
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

				Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
				if (currMat->HasColor())
				{
					color = currMat->GetColor();
				}

				shader->SetFloat4("u_matColor", color);

				ResourceHandler::GetTexture2D(diffuseMap)->Bind();
				ResourceHandler::GetTexture2D(specularMap)->Bind(1);

				pair.GetElement().RenderGeometry(viewProj);
			}
			m_gBuffer->Unbind();
			RenderCommand::EnableBlending(true);
			RenderCommand::SetClearColor(clearColor);
			RenderCommand::Clear();

			shader = ResourceHandler::GetShader(m_deferredShader);
			shader->Bind();
			Renderer::SendLightUniformsToShader(shader);
			shader->SetFloat3("u_camPos", Renderer::GetCamPos());
			m_gBuffer->BindTexureData();

			m_deferredVB->Bind();
			m_deferredIB->Bind();
			RenderCommand::DrawIndexed(m_deferredIB);
			m_gBuffer->GetFramebuffer()->CopyTo(nullptr);
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


	// LightData ///////////////////////////////////////////
	LightData::LightData() : m_type(LightType::Directional) { }
	LightData::LightData(const Vector3& position, const Vector3& color) : m_position(position), m_color(color),
		m_diffuseIntensity(0.75f), m_specularIntensity(1.0f), m_type(LightType::Directional), 
		m_radius(3.0f), m_innerAngle(30.0f), m_outerAngle(m_innerAngle + 1.0f) { }

	LightType LightData::GetLightType() const { return m_type; }
	const Vector3& LightData::GetPosition() const { return m_position; }
	const Vector3& LightData::GetDirection() const { return m_direction; }
	const Vector3& LightData::GetColor() const { return m_color; }
	const Vector3& LightData::GetDiffuseColor() const { return m_color * m_diffuseIntensity; }
	const Vector3& LightData::GetSpecularColor() const { return m_color * m_specularIntensity; }
	float LightData::GetDiffuseIntensity() const { return m_diffuseIntensity; }
	float LightData::GetSpecularIntensity() const { return m_specularIntensity; }
	float LightData::GetRadius() const { return m_radius; }
	float LightData::GetInnerAngle() const { return m_innerAngle; }
	float LightData::GetOuterAngle() const { return m_outerAngle; }

	void LightData::SetLightType(LightType type) { m_type = type; }

	void LightData::SetPosition(const Vector3& position) { m_position = position; }

	void LightData::SetDirection(const Vector3& direction)
	{
		AE_CORE_ASSERT(direction != Vector3::Zero(), "Invalid direction provided to light");
		m_direction = direction;
	}

	void LightData::SetColor(const Vector3& color) { m_color = color; }
	void LightData::SetDiffuseIntensity(float intensity) { m_diffuseIntensity = intensity; }
	void LightData::SetSpecularIntensity(float intensity) { m_specularIntensity = intensity; }
	void LightData::SetRadius(float radius) { m_radius = radius; }
	void LightData::SetInnerAngle(float angle) { m_innerAngle = angle; }
	void LightData::SetOuterAngle(float angle) { m_outerAngle = angle; }

	// LightHandler //////////////////////////////////////////////////////////////////
	LightHandler::LightHandler() : m_lights(s_maxNumLights), m_lightsModified(false) { }
	
	LightHandle LightHandler::AddLight(LightData& data)
	{
		if (m_lights.GetCount() >= s_maxNumLights && m_handlesToRecycle.IsEmpty())
		{
			AE_CORE_WARN("Scene already contains the maximum number of lights supported. Cannot add more lights");
			return NullHandle;
		}
		m_lightsModified = true;

		LightHandle light = NullHandle;
		if (m_handlesToRecycle.IsEmpty())
		{
			light = m_lights.GetCount();
			m_lights.Add(std::move(data));
		}
		else
		{
			light = m_handlesToRecycle.Pop();
			m_lights[light] = std::move(data);
		}

		switch(m_lights[light].GetLightType())
		{
		case LightType::Directional:
			m_directionalLights.Add(light);
			break;

		case LightType::Point:
			m_pointLights.Add(light);
			break;

		case LightType::Spot:
			m_spotLights.Add(light);
			break;

		default:
			AE_CORE_ERROR("Unknown light type detected");
			break;
		}

		return light;
	}
	
	void LightHandler::RemoveLight(LightHandle light)
	{
		if (LightIsValid(light))
		{
			m_lightsModified = true;
			LightData& data = GetLightData(light);
			switch(data.GetLightType())
			{
			case LightType::Directional:
				m_directionalLights.Remove(light);
				break;

			case LightType::Point:
				m_pointLights.Remove(light);
				break;

			case LightType::Spot:
				m_spotLights.Remove(light);
				break;

			default:
				AE_CORE_ERROR("Unknown light type detected");
				break;
			}
			m_handlesToRecycle.Push(light);
		}
	}

	LightData& LightHandler::GetLightData(LightHandle light)
	{
		AE_CORE_ASSERT(LightIsValid(light), "");
		m_lightsModified = true;
		return m_lights[light];
	}

	const LightData& LightHandler::GetLightData(LightHandle light) const
	{
		AE_CORE_ASSERT(LightIsValid(light), "");
		return m_lights[light];
	}

	bool LightHandler::LightIsValid(LightHandle light) const
	{
		return light < m_lights.GetCount() && !m_handlesToRecycle.Contains(light);
	}

	bool LightHandler::LightsModified() const { return m_lightsModified; }

	void LightHandler::SendLightUniformsToShader(AReference<Shader>& shader) const
	{
		if (!LightsModified() || shader == nullptr)
		{
			return;
		}
		
		SendDirectionalLightUniforms(shader);
		SendPointLightUniforms(shader);
		SendSpotLightUniforms(shader);
	}

	void LightHandler::SendDirectionalLightUniforms(AReference<Shader>& shader) const
	{
		shader->SetInt("u_numDirectionalLights", m_directionalLights.GetCount());
		size_t index = 0;
		std::stringstream ss;
		for (LightHandle light : m_directionalLights)
		{
			const LightData& data = GetLightData(light);
			ss.str("");
			ss << "u_directionalLightArr[" << index << "]";
			std::string varName = ss.str();
			
			ss.str("");
			ss << varName << ".direction";
			shader->SetFloat3(ss.str(), data.GetDirection());

			ss.str("");
			ss << varName << ".diffuse";
			shader->SetFloat3(ss.str(), data.GetDiffuseColor());

			ss.str("");
			ss << varName << ".specular";
			shader->SetFloat3(ss.str(), data.GetSpecularColor());

			index++;
		}
	}

	void LightHandler::SendPointLightUniforms(AReference<Shader>& shader) const
	{
		shader->SetInt("u_numPointLights", m_pointLights.GetCount());
		size_t index = 0;
		std::stringstream ss;
		for (LightHandle light : m_pointLights)
		{
			const LightData& data = GetLightData(light);
			ss.str("");
			ss << "u_pointLightArr[" << index << "]";
			std::string varName = ss.str();

			ss.str("");
			ss << varName << ".position";
			shader->SetFloat3(ss.str(), data.GetPosition());

			ss.str("");
			ss << varName << ".diffuse";
			shader->SetFloat3(ss.str(), data.GetDiffuseColor());

			ss.str("");
			ss << varName << ".specular";
			shader->SetFloat3(ss.str(), data.GetSpecularColor());

			ss.str("");
			ss << varName << ".radius";
			shader->SetFloat(ss.str(), data.GetRadius());

			index++;
		}
	}

	void LightHandler::SendSpotLightUniforms(AReference<Shader>& shader) const
	{
		shader->SetInt("u_numSpotLights", m_spotLights.GetCount());
		size_t index = 0;
		std::stringstream ss;
		for (LightHandle light : m_spotLights)
		{
			const LightData& data = GetLightData(light);
			ss.str("");
			ss << "u_spotLightArr[" << index << "]";
			std::string varName = ss.str();

			ss.str("");
			ss << varName << ".position";
			shader->SetFloat3(ss.str(), data.GetPosition());

			ss.str("");
			ss << varName << ".direction";
			shader->SetFloat3(ss.str(), data.GetDirection());

			ss.str("");
			ss << varName << ".diffuse";
			shader->SetFloat3(ss.str(), data.GetDiffuseColor());

			ss.str("");
			ss << varName << ".specular";
			shader->SetFloat3(ss.str(), data.GetSpecularColor());

			ss.str("");
			ss << varName << ".innerAngle";
			shader->SetFloat(ss.str(), Math::Cos(Math::DegreeToRadians(data.GetInnerAngle())));

			ss.str("");
			ss << varName << ".outerAngle";
			shader->SetFloat(ss.str(), Math::Cos(Math::DegreeToRadians(data.GetOuterAngle())));

			ss.str("");
			ss << varName << ".radius";
			shader->SetFloat(ss.str(), data.GetRadius());

			index++;
		}
	}

	void LightHandler::OnLightTypeChange(LightHandle light, LightType oldType, LightType newType)
	{
		AE_CORE_ASSERT(LightIsValid(light), "");
		switch(oldType)
		{
		case LightType::Directional:
			m_directionalLights.Remove(light);
			break;

		case LightType::Point:
			m_pointLights.Remove(light);
			break;

		case LightType::Spot:
			m_spotLights.Remove(light);
			break;

		default:
			AE_CORE_ERROR("Unknown light type detected");
			break;
		}

		switch(newType)
		{
		case LightType::Directional:
			m_directionalLights.Add(light);
			break;

		case LightType::Point:
			m_pointLights.Add(light);
			break;

		case LightType::Spot:
			m_spotLights.Add(light);
			break;

		default:
			AE_CORE_ERROR("Unknown light type detected");
			break;
		}
	}
}