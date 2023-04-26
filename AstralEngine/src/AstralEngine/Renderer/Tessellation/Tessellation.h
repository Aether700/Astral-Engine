#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Core/Resource.h"

namespace AstralEngine
{
	class Tessellation
	{
	public:
		// uses the Boyer-Watson algorithm to generate a triangulation of the provided list of points.
		static MeshHandle BoyerWatson(const ADynArr<Vector2>& points);

		https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
		needs to compute bridge edge between hole contour and shape contour
		static MeshHandle EarClipping(<not determined yet>);
	};
}