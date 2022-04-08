#include "aepch.h"
#include "Physics2DCore.h"

namespace AstralEngine
{
	// AxisAlignedBoundingBox2D ///////////////////////////////////////////
	
	AxisAlignedBoundingBox2D::AxisAlignedBoundingBox2D() { }
	AxisAlignedBoundingBox2D::AxisAlignedBoundingBox2D(const Vector2& min, const Vector2& max) : m_min(min), m_max(max) { }

	AxisAlignedBoundingBox2D AxisAlignedBoundingBox2D::Union(const AxisAlignedBoundingBox2D& other) const
	{
		return AxisAlignedBoundingBox2D(Vector2::Min(m_min, other.m_min), Vector2::Max(m_max, other.m_max));
	}

	float AxisAlignedBoundingBox2D::Area() const
	{
		return (m_max.x - m_min.x) * (m_max.y - m_min.y);
	}

}