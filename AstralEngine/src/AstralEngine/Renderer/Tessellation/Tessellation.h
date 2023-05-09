#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Core/Resource.h"
#include "AstralEngine/Renderer/Mesh.h"

namespace AstralEngine
{
	class Tessellation
	{
	public:
		// uses the Boyer-Watson algorithm to generate a triangulation of the provided list of points.
		static MeshHandle BoyerWatson(const ADynArr<Vector2>& points);

		/*
		https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
		needs to compute bridge edge between hole contour and shape contour
		to find bridge edge use algorithm in link above. For "raycasting" part see
		https://www.jeffreythompson.org/collision-detection/line-line.php#:~:text=To%20check%20if%20two%20lines,x3)*(y2-y1))%3B
		*/
		static MeshHandle EarClipping(const ASinglyLinkedList<ADynArr<Vector2>>& points);

	private:

		// ear clipping helpers

		static ADoublyLinkedListIterator<ADoublyLinkedList<Vector2>> 
			FindInnerPolygonWithRightMostVertex(ADoublyLinkedList<ADoublyLinkedList<Vector2>>& innerPolygonList);
		// checks if first two points' visibility is blocked by the edge of the last two points
		static bool EdgeBlocksVisibility(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4);

		// p1 is a point from ring1 and p2 is a point from ring2
		static bool IsValidBridgePair(ADoublyLinkedList<Vector2>::AIterator& p1,
			ADoublyLinkedList<Vector2>::AIterator& p2, ADoublyLinkedList<Vector2>& ring1, ADoublyLinkedList<Vector2>& ring2);
		static ADoublyLinkedList<Vector2> BuildBridge(ADoublyLinkedList<Vector2>& innerPolygon, ADoublyLinkedList<Vector2>& outerPolygon);
		static void ClipEars(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh);
		
	};
}