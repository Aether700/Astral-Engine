#include "aepch.h"
#include "AstralEngine/Core/Application.h"
#include "Rigidbody2D.h"
#include "PhysicsEngine2D.h"

namespace AstralEngine
{
	Rigidbody2D::Rigidbody2D()
	{
		Application::GetPhysicsEngine2D()->m_rigidbodies.Add(this);
	}

	Rigidbody2D::~Rigidbody2D()
	{
		Application::GetPhysicsEngine2D()->m_rigidbodies.Remove(this);
	}

	void Rigidbody2D::OnUpdate()
	{
		GetTransform().SetLocalPosition(m_position);
		GetTransform().SetRotation(0.0f, 0.0f, m_rotation);
	}
}