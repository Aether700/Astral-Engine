#pragma once
#include "AstralEngine/Core/Layer.h"
#include "Rigidbody2D.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"

namespace AstralEngine
{
	class PhysicsEngine2D : public Layer
	{
		friend class Rigidbody2D;
	public:
		void OnUpdate() override;

	private:		
		void ApplyForces();
		void UpdatePositions();

		Vector2 m_gravity = Vector2(0.0f, -9.8f);
		ASinglyLinkedList<Rigidbody2D*> m_rigidbodies;
	};
}