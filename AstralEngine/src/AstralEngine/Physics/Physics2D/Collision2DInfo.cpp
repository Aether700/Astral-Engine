#include "aepch.h"
#include "Collision2DInfo.h"

namespace AstralEngine
{
	Collision2DInfo::Collision2DInfo(const Vector3& normal, float depth) 
		: m_normal(normal), m_depth(depth) { }

	const Vector3& Collision2DInfo::GetNormal() const { return m_normal; }
	const float Collision2DInfo::GetDepth() const { return m_depth; }
}