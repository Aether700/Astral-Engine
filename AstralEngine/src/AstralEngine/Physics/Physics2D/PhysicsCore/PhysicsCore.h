#pragma once
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	//common data structure for both 2D and 3D physics. Takes an Axis Align Bounding Box (AABB) as type parameter

	template<typename T>
	class DynamicAABBTree
	{
	public:
		void InsertAABB(const T& aabb);

	private:
	};
}