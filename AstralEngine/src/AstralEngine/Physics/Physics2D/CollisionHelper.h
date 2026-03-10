#pragma once
#include "Collider2D.h"

namespace AstralEngine 
{

	// static class containing helper functions to determine if a collision occured between primitives
	class CollisionHelper
	{
	public:
		static bool BoxToBoxCollision(const BoxCollider2D& b1, const BoxCollider2D& b2);
	};
}