#include "aepch.h"
#include "PhysicsEngine2D.h"
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	void PhysicsEngine2D::OnUpdate()
	{
		ApplyForces();
		UpdatePositions();
		// check for collision here
	}

	void PhysicsEngine2D::ApplyForces()
	{
		for(Rigidbody2D* rb : m_rigidbodies)
		{
			Vector2 gravityForce = m_gravity * rb->GetGravityScale(); //temp
			rb->AddForce(m_gravity * rb->GetGravityScale());
			Vector2 forceToApply = rb->m_forceToApplyThisFrame;

			//Should use different time step(?)
			rb->SetVelocity(rb->GetVelocity() + (forceToApply / rb->GetMass()) * Time::GetDeltaTime());
			rb->m_forceToApplyThisFrame = Vector2::Zero();
		}
	}

	void PhysicsEngine2D::UpdatePositions()
	{
		for (Rigidbody2D* rb : m_rigidbodies)
		{
			//Should use different time step(?)
			rb->SetPosition(rb->GetPosition() + rb->GetVelocity() * Time::GetDeltaTime());
		}
	}
}