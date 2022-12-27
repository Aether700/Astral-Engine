#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"

namespace AstralEngine
{
	class Tessellation
	{
	public:
		// uses the Boyer-Watson algorithm to generate a triangulation of the provided list of points.
		static void BoyerWatson(const ASinglyLinkedList<Vector2>& points, 
			ASinglyLinkedList<Vector2>& outVertices, ASinglyLinkedList<unsigned int>& outIndices);
	};
}