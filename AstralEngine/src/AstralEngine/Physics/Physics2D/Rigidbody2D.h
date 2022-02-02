#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/Components.h"

namespace AstralEngine
{
	//enum used as flags to set constraints on a rigidbody2d
	enum Rigidbody2DConstraints
	{
		Rigidbody2DConstraintsNone = 0,
		Rigidbody2DConstraintsFixedXPosition = 1,
		Rigidbody2DConstraintsFixedYPosition = 1 << 1,
		Rigidbody2DConstraintsFixedRotation = 1 << 2,
		Rigidbody2DConstraintsFixedPosition = Rigidbody2DConstraintsFixedXPosition 
			| Rigidbody2DConstraintsFixedYPosition,
	};

	class Rigidbody2D : public NativeScript
	{
		friend class PhysicsEngine2D;
	public:
		Rigidbody2D();
		~Rigidbody2D();

		Vector2& GetPosition() { return m_position; }
		const Vector2& GetPosition() const { return m_position; }
		void SetPosition(const Vector2& position) { m_position = position; }

		Vector2& GetVelocity() { return m_velocity; }
		const Vector2& GetVelocity() const { return m_velocity; }
		void SetVelocity(const Vector2& velocity) { m_velocity = velocity; }

		float GetAngularVelocity() const { return m_angularVelocity; }
		void SetAngularVelocity(float angularVelocity) { m_angularVelocity = angularVelocity; }

		float GetRotation() const { return m_rotation; }
		void SetRotation(float rotation) { m_rotation = rotation; }
		float GetMass() const { return m_mass; }
		void SetMass(float mass) { m_mass = mass; }

		float GetInertia() const { return m_inertia; }
		void SetInertia(float inertia) 
		{ 
			m_inertia = inertia; 
			if (m_inertia == 0.0f)
			{
				m_inertia = 1.0f;
			}
		}
		
		float GetGravityScale() const { return m_gravityScale; }
		void SetGravityScale(float scale) { m_gravityScale = scale; }

		void AddForce(const Vector2& force)
		{
			if (!(m_constraints & Rigidbody2DConstraintsFixedPosition))
			{
				m_forceToApplyThisUpdate += force;
				if (m_constraints * Rigidbody2DConstraintsFixedXPosition)
				{
					m_forceToApplyThisUpdate.x = 0.0f;
				}
				else if (m_constraints * Rigidbody2DConstraintsFixedYPosition)
				{
					m_forceToApplyThisUpdate.y = 0.0f;
				}
			}
		}
		void AddTorque(float torque) 
		{
			if (m_constraints & Rigidbody2DConstraintsFixedRotation)
			{
				return;
			}
			m_torqueToApplyThisUpdate += torque; 
		}

		Rigidbody2DConstraints GetContraints() const { return m_constraints; }
		void SetConstraints(int contraints) 
		{ 
			m_constraints = (Rigidbody2DConstraints)contraints;

			if (m_constraints & Rigidbody2DConstraintsFixedPosition)
			{
				m_velocity = Vector2::Zero();
			}
			else if (m_constraints & Rigidbody2DConstraintsFixedXPosition)
			{
				m_velocity.x = 0.0f;
			}
			else if (m_constraints & Rigidbody2DConstraintsFixedYPosition)
			{
				m_velocity.y = 0.0f;
			}

			if (m_constraints & Rigidbody2DConstraintsFixedRotation)
			{
				m_angularVelocity = 0.0f;
			}
		}

		void OnUpdate();

	private:		
		Vector2 m_position;
		Vector2 m_velocity;
		float m_rotation = 0.0f; //in degrees
		float m_angularVelocity = 0.0f; //in radians per sec
		float m_mass = 1.0f;
		float m_gravityScale = 1.0f;
		float m_inertia = 1.0f;

		Rigidbody2DConstraints m_constraints = Rigidbody2DConstraintsNone;

		Vector2 m_forceToApplyThisUpdate = Vector2::Zero();
		float m_torqueToApplyThisUpdate = 0.0f;
	};
}