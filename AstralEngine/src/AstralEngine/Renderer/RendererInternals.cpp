#include "aepch.h"
#include "RendererInternals.h"

namespace AstralEngine
{
	// DrawCommand /////////////////////////////////
	
	DrawCommand::DrawCommand() { }
	DrawCommand::DrawCommand(const Mat4& transform, MaterialHandle mat, RenderableType type)
		: m_transform(transform), m_type(type), m_material(mat) { }

	const Mat4& DrawCommand::GetTransform() const { return m_transform; }
	MaterialHandle DrawCommand::GetMaterial() const { return m_material; }
	RenderableType DrawCommand::GetType() const { return m_type; }
	bool DrawCommand::IsOpaque() const { return ResourceHandler::GetMaterial(m_material)->GetColor().a == 1.0f; }

	bool DrawCommand::operator==(const DrawCommand& other) const
	{
		return m_type == other.m_type && m_material == other.m_material
			&& m_transform == other.m_transform;
	}

	bool DrawCommand::operator!=(const DrawCommand& other) const
	{
		return !(*this == other);
	}

	// DrawCallList /////////////////////////////////////////////////
	DrawCallList::DrawCallList() : m_material(nullHandle) { }
	DrawCallList::DrawCallList(MaterialHandle material, RenderableType type) : m_material(material), m_type(type) { }
	DrawCallList::~DrawCallList()
	{
		Clear();
	}

	MaterialHandle DrawCallList::GetMaterial() const { return m_material; }

	RenderableType DrawCallList::GetType() const { return m_type; }

	void DrawCallList::AddDrawCommand(DrawCommand* draw)
	{
		AE_CORE_ASSERT(draw->GetMaterial() == m_material && draw->GetType() == m_type, "");
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

}