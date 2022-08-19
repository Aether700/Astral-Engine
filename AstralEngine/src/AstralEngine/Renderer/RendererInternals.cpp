#include "aepch.h"
#include "RendererInternals.h"

namespace AstralEngine
{
	// DrawCommand /////////////////////////////////
	
	DrawCommand::DrawCommand() { }
	DrawCommand::DrawCommand(const Mat4& transform, MaterialHandle mat, MeshHandle mesh)
		: m_transform(transform), m_mesh(mesh), m_material(mat) { }

	const Mat4& DrawCommand::GetTransform() const { return m_transform; }
	MaterialHandle DrawCommand::GetMaterial() const { return m_material; }
	MeshHandle DrawCommand::GetMesh() const { return m_mesh; }
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

				Mat4* transformArr = new Mat4[m_data.GetCount()];
				size_t index = 0;
				for (DrawCommand* cmd : m_data)
				{
					transformArr[index] = cmd->GetTransform();
					index++;
				}

				m_geometryDataBuffer->Bind();
				m_instanceArrBuffer->Bind();
				m_instanceArrBuffer->SetData(transformArr, sizeof(Mat4) * m_data.GetCount());
				delete[] transformArr;

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
		m_instanceArrBuffer->SetLayout({ { ADataType::Mat4, "transformMatrix", 1 } }, 1, sizeof(Vector3));

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
		if (!m_drawCalls.ContainsKey(data->GetMaterial()))
		{
			m_drawCalls.Add(data->GetMaterial(), DrawCallList(data->GetMaterial(), data->GetMesh()));
		}
		m_drawCalls[data->GetMaterial()].AddDrawCommand(data);
	}

	void RenderingDataSorter::Clear()
	{
		for (auto& pair : m_drawCalls)
		{
			pair.GetElement().Clear();
		}
	}

	RenderingDataSorter::AIterator RenderingDataSorter::begin()
	{
		return m_drawCalls.begin();
	}

	RenderingDataSorter::AIterator RenderingDataSorter::end()
	{
		return m_drawCalls.end();
	}

	RenderingDataSorter::AConstIterator RenderingDataSorter::begin() const
	{
		return m_drawCalls.begin();
	}

	RenderingDataSorter::AConstIterator RenderingDataSorter::end() const
	{
		return m_drawCalls.end();
	}

}