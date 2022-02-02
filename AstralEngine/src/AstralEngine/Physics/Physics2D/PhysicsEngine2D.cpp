#include "aepch.h"
#include "PhysicsEngine2D.h"
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	void PhysicsEngine2D::OnUpdate()
	{
		ApplyForcesAndTorque();
		UpdatePositions();
		// check for collision here
	}

	void PhysicsEngine2D::ApplyForcesAndTorque()
	{
		//Should use different time step(?)
		float timeStep = Time::GetDeltaTime();
		for(Rigidbody2D* rb : m_rigidbodies)
		{
			rb->AddForce(m_gravity * rb->GetGravityScale());
			Vector2 forceToApply = rb->m_forceToApplyThisUpdate;
			rb->SetVelocity(rb->GetVelocity() + (forceToApply / rb->GetMass()) * timeStep);

			rb->m_forceToApplyThisUpdate = Vector2::Zero();
			rb->SetAngularVelocity(rb->GetAngularVelocity() 
				+ (rb->m_torqueToApplyThisUpdate * timeStep) / rb->m_inertia);
			rb->m_torqueToApplyThisUpdate = 0.0f;
		}
	}

	void PhysicsEngine2D::UpdatePositions()
	{
		//Should use different time step(?)
		float timeStep = Time::GetDeltaTime();
		for (Rigidbody2D* rb : m_rigidbodies)
		{
			rb->SetPosition(rb->GetPosition() + rb->GetVelocity() * timeStep);
			rb->SetRotation(rb->GetRotation() + Math::RadiantsToDegree(rb->GetAngularVelocity() * timeStep));
		}
	}
}