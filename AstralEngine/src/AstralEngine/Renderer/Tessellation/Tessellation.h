#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"
#include "AstralEngine/Data Struct/ADynArr.h"

namespace AstralEngine
{
	class Tessellation
	{
	public:
		// uses the Boyer-Watson algorithm to generate a triangulation of the provided list of points.
		static void BoyerWatson(const ASinglyLinkedList<Vector2>& points, 
			ADynArr<Vector2>& outVertices, ADynArr<unsigned int>& outIndices);
	};
}