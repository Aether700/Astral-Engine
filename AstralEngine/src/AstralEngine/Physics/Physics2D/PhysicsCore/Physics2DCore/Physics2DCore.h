#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class AxisAlignedBoundingBox2D
	{
	public:
		AxisAlignedBoundingBox2D();
		AxisAlignedBoundingBox2D(const Vector2& min, const Vector2& max);

		Vector2 GetMin() const { return m_min; }
		Vector2 GetMax() const { return m_max; }

		AxisAlignedBoundingBox2D Union(const AxisAlignedBoundingBox2D& other) const;
		float Area() const;

	private:
		Vector2 m_min;
		Vector2 m_max;
	};
}