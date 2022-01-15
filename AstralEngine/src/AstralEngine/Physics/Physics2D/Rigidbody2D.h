#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/ECS/Components.h"

namespace AstralEngine
{
	class Rigidbody2D
	{
		friend class PhysicsEngine2D;
		friend class Collider2D;
	public:
		Rigidbody2D();
		~Rigidbody2D();

		Vector2& GetPosition() { return m_position; }
		const Vector2& GetPosition() const { return m_position; }
		void SetPosition(const Vector2& position) { m_position = position; }

		Vector2& GetVelocity() { return m_velocity; }
		const Vector2& GetVelocity() const { return m_velocity; }
		void SetVelocity(const Vector2& velocity) { m_velocity = velocity; }

		float GetRotation() const { return m_rotation; }
		float GetMass() const { return m_mass; }

		float GetGravityScale() const { return m_gravityScale; }
		void SetGravityScale(float scale) { m_gravityScale = scale; }

		void AddForce(const Vector2& force) { m_forceToApplyThisFrame += force; }
		void AddTorque(float torque) { m_torqueToApplyThisFrame += torque; }

	private:		

		Vector2 m_position;
		Vector2 m_velocity;
		float m_rotation = 0.0f; //in radians
		float m_angularVelocity = 0.0f; //in radians per sec
		float m_mass = 1.0f;
		float m_gravityScale = 1.0f;

		Vector2 m_forceToApplyThisFrame = Vector2::Zero();
		float m_torqueToApplyThisFrame = 0.0f;
		Collider2D* m_collider = nullptr;
	};
}