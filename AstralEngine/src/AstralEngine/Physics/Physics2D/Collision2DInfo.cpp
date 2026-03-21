#include "aepch.h"
#include "Collision2DInfo.h"

namespace AstralEngine
{
	Collision2DInfo::Collision2DInfo(const Vector3& normal, float depth, ADynArr<Vector2>&& collisionPoints) 
		: m_normal(normal), m_depth(depth), m_collisionPoints(std::move(collisionPoints)) { }

	const Vector3& Collision2DInfo::GetNormal() const { return m_normal; }
	const float Collision2DInfo::GetDepth() const { return m_depth; }
	const ADynArr<Vector2>& Collision2DInfo::GetCollisionPoints() const { return m_collisionPoints; }
}