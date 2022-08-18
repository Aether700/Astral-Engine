#include "aepch.h"
#include "RendererInternals.h"

namespace AstralEngine
{
	// DrawCommand /////////////////////////////////
	
	DrawCommand::DrawCommand() { }
	DrawCommand::DrawCommand(const Mat4& transform, AReference<Material> mat, RenderableType type)
		: m_transform(transform), m_type(type), m_material(mat) { }

	const Mat4& DrawCommand::GetTransform() const { return m_transform; }
	const AReference<Material>& DrawCommand::GetMaterial() const { return m_material; }
	RenderableType DrawCommand::GetType() const { return m_type; }
	bool DrawCommand::IsOpaque() const { return m_material->GetColor().a == 1.0f; }

	bool DrawCommand::operator==(const DrawCommand& other) const
	{
		return m_type == other.m_type && m_material == other.m_material
			&& m_transform == other.m_transform;
	}

	bool DrawCommand::operator!=(const DrawCommand& other) const
	{
		return !(*this == other);
	}
}