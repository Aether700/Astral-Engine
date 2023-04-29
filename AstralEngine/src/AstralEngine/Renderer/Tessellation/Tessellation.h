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
		static void EarClippingImpl(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh);
	};
}