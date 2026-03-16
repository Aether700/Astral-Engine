#include "aepch.h"
#include "AstralEngine/Core/Application.h"
#include "Rigidbody2D.h"
#include "PhysicsEngine2D.h"

namespace AstralEngine
{
	Rigidbody2D::Rigidbody2D() { }

	Rigidbody2D::~Rigidbody2D() { }

	void Rigidbody2D::OnUpdate()
	{
		GetTransform().SetLocalPosition(m_position);
		GetTransform().SetRotation(0.0f, 0.0f, m_rotation);
	}
}