#include "aepch.h"
#include "Tessellation.h"
#include "AstralEngine/Renderer/Mesh.h"

namespace AstralEngine
{
	size_t ComputeSuperTriangle(MeshDataView& data, const ASinglyLinkedList<Vector2>& points)
	{
		Vector2 rectangleMin = points[0];
		Vector2 rectangleMax = points[0];

		for (const Vector2& point : points)
		{
			if (point.x < rectangleMin.x) { rectangleMin.x = point.x; }
			if (point.y < rectangleMin.y) { rectangleMin.y = point.y; }
			if (point.x > rectangleMax.x) { rectangleMin.x = point.x; }
			if (point.y > rectangleMax.y) { rectangleMin.y = point.y; }
		}

		float rectangleWidth = rectangleMax.x - rectangleMin.x;
		float rectangleHeight = rectangleMax.y - rectangleMin.y;
		float biggerSide = Math::Max(rectangleWidth, rectangleHeight);
		float midWidth = (rectangleMax.x + rectangleMin.x) / 2.0f;
		float midHeight = (rectangleMax.y + rectangleMin.y) / 2.0f;

		return data.AddTriangle(
			Vector3(midWidth - 20.0f * biggerSide, midHeight - biggerSide, 0.0f), 
			Vector3(midWidth, midHeight + 20.0f * biggerSide, 0.0f), 
			Vector3(midWidth + 20.0f * biggerSide, midHeight - biggerSide, 0.0f));
	}


	void Tessellation::BoyerWatson(const ASinglyLinkedList<Vector2>& points,
		ASinglyLinkedList<Vector2>& vertices, ASinglyLinkedList<unsigned int>& indices)
	{
		MeshDataView dataView;
		size_t superTriangle = ComputeSuperTriangle(dataView, points);
		ASinglyLinkedList<size_t> triangulation;
		triangulation.Add(superTriangle);

		for (const Vector2& point : points)
		{
			ASinglyLinkedList<size_t> badTriangles;

			// find all invalid triangles
			for (size_t& triangle : triangulation)
			{
				if (dataView.PointIsInTriangleCircumsphere(triangle, point))
				{
					badTriangles.Add(triangle);
				}
			}

			//construct edge list of the polygon created by the invalid triangles
			ASinglyLinkedList<size_t> polygon;

			for (size_t& triangle : badTriangles)
			{
				not finished need a utility function to query view and figure out if the edge is shared by a triangle
				/*
				if (IsEdgeSharedByOtherTriangles(badTriangles, &triangle, triangle.GetPointA(), triangle.GetPointB()))
				{
					polygon.Emplace(triangle.GetPointA(), triangle.GetPointB());
				}

				if (IsEdgeSharedByOtherTriangles(badTriangles, &triangle, triangle.GetPointA(), triangle.GetPointC()))
				{
					polygon.Emplace(triangle.GetPointA(), triangle.GetPointC());
				}

				if (IsEdgeSharedByOtherTriangles(badTriangles, &triangle, triangle.GetPointB(), triangle.GetPointC()))
				{
					polygon.Emplace(triangle.GetPointB(), triangle.GetPointC());
				}
				*/
			}

			// remove the invalid triangles from the triangulation
			for (TriangleData& triangle : badTriangles)
			{
				triangulation.Remove(triangle);
			}

			// create and add triangles between the point of the point list and the edges of the polygon left from the invalid triangles
			for (EdgeData& edge : polygon)
			{
				triangulation.Emplace(edge.point1, edge.point2, point);
			}
		}
	}
}