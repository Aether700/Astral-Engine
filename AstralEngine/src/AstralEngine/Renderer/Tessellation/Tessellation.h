#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Core/Resource.h"
#include "AstralEngine/Renderer/Mesh.h"

namespace AstralEngine
{
	enum class TessellationWindingOrder
	{
		// indicates that the outer contour of the polynomial is going in clockwise direction and any 
		// inner polygons are in counter clockwise direction
		ClockWise, 
		
		// opposite of the above
		CounterClockWise
	};

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
		static MeshHandle EarClipping(const ADoublyLinkedList<ADynArr<Vector2>>& points, 
			TessellationWindingOrder windingOrder = TessellationWindingOrder::CounterClockWise);

	private:

		// ear clipping helpers

		static Vector3Int GetEarIndices(size_t listSize, size_t tipIndex, TessellationWindingOrder windingOrder);
		static bool IsAnEar(const ADoublyLinkedList<Vector2>& points, const Vector3Int& ear);
		// returns the indices of the points forming the ear stored in a Vector3Int with the tip of the 
		// ear being stored in the y component of the vector. returns Vector3Int::Zero if no ear is found
		static Vector3Int FindEar(const ADoublyLinkedList<Vector2>& points, TessellationWindingOrder windingOrder);
		static ADoublyLinkedListIterator<ADoublyLinkedList<Vector2>> 
			FindInnerPolygonWithRightMostVertex(ADoublyLinkedList<ADoublyLinkedList<Vector2>>& innerPolygonList);
		// checks if first two points' visibility is blocked by the edge of the last two points
		static bool EdgeBlocksVisibility(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4);

		// p1 is a point from ring1 and p2 is a point from ring2
		static bool IsValidBridgePair(ADoublyLinkedList<Vector2>::AIterator& p1,
			ADoublyLinkedList<Vector2>::AIterator& p2, ADoublyLinkedList<Vector2>& ring1, ADoublyLinkedList<Vector2>& ring2);
		static ADoublyLinkedList<Vector2> BuildBridge(ADoublyLinkedList<Vector2>& innerPolygon, ADoublyLinkedList<Vector2>& outerPolygon);
		static bool ClipEars(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh, 
			TessellationWindingOrder windingOrder);
		
	};
}