#include "aepch.h"
#include "PhysicsEngine2D.h"
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	void PhysicsEngine2D::OnUpdate()
	{
		ApplyForcesAndTorque();
		UpdatePositions();
		double check logic of statement in comment below
		// check for collision here 
	}

	collision and contact points computed for squares now time to apply forces and torque in a realistic manner
		good reference but might not explain rotation of bodies depending on contact point https://www.toptal.com/developers/game/video-game-physics-part-iii-constrained-rigid-body-simulation
	lecture referenced by Jolt physics engine: https://box2d.org/files/ErinCatto_ModelingAndSolvingConstraints_GDC2009.pdf

	void PhysicsEngine2D::ApplyForcesAndTorque()
	{
		//Should use different time step(?)
		float timeStep = Time::GetDeltaTime();
		auto group = m_scene->m_registry.GetView<Rigidbody2D>();

		for (BaseEntity e : group)
		{
			Rigidbody2D& rb = m_scene->m_registry.GetComponent<Rigidbody2D>(e);
			rb.AddForce(m_gravity * rb.GetGravityScale());
			Vector2 forceToApply = rb.m_forceToApplyThisUpdate;
			rb.SetVelocity(rb.GetVelocity() + (forceToApply / rb.GetMass()) * timeStep);

			rb.m_forceToApplyThisUpdate = Vector2::Zero();
			rb.SetAngularVelocity(rb.GetAngularVelocity() 
				+ (rb.m_torqueToApplyThisUpdate * timeStep) / rb.m_inertia);
			rb.m_torqueToApplyThisUpdate = 0.0f;
		}
	}

	void PhysicsEngine2D::UpdatePositions()
	{
		//Should use different time step(?)
		float timeStep = Time::GetDeltaTime();

		auto group = m_scene->m_registry.GetView<Rigidbody2D>();

		for (BaseEntity e : group)
		{
			Rigidbody2D& rb = m_scene->m_registry.GetComponent<Rigidbody2D>(e);
			Rigidbody2DConstraints constraints = rb.GetContraints();
			if (!(constraints & Rigidbody2DConstraintsFixedPosition))
			{
				Vector2 velocity = rb.GetVelocity();

				if (constraints & Rigidbody2DConstraintsFixedXPosition)
				{
					velocity.x = 0.0f;
				}

				if (constraints & Rigidbody2DConstraintsFixedYPosition)
				{
					velocity.y = 0.0f;
				}

				rb.SetPosition(rb.GetPosition() + velocity * timeStep);
			}

			if (!(constraints & Rigidbody2DConstraintsFixedRotation)) 
			{
				rb.SetRotation(rb.GetRotation() + Math::RadiansToDegree(rb.GetAngularVelocity() * timeStep));
			}
		}
	}
}