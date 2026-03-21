#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	class Collision2DInfo
	{
	public:
		Collision2DInfo(const Vector3& normal, float depth, ADynArr<Vector2>&& collisionPoints);

		const Vector3& GetNormal() const;
		const float GetDepth() const;
		const ADynArr<Vector2>& GetCollisionPoints() const;

	private:
		Vector2 m_normal;
		float m_depth;
		ADynArr<Vector2> m_collisionPoints;
	};
}