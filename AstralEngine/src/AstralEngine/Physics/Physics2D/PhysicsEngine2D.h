#pragma once
#include "AstralEngine/Core/Layer.h"
#include "Rigidbody2D.h"
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"
#include "AstralEngine/ECS/Scene.h"

namespace AstralEngine
{
	class PhysicsEngine2D : public Layer
	{
	public:
		void OnUpdate() override;

		void SetScene(Scene* s) { m_scene = s; }

	private:		
		void ApplyForcesAndTorque();
		void UpdatePositions();

		Vector2 m_gravity = Vector2(0.0f, -9.8f);
		Scene* m_scene;
	};
}