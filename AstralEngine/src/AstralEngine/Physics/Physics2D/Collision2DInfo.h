#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class Collision2DInfo
	{
	public:
		Collision2DInfo(const Vector3& normal, float depth);

		const Vector3& GetNormal() const;
		const float GetDepth() const;

	private:
		Vector2 m_normal;
		float m_depth;
	};
}