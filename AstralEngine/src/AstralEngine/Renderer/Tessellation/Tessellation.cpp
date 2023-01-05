#include "aepch.h"
#include "Tessellation.h"
#include "AstralEngine/Renderer/Mesh.h"

namespace AstralEngine
{
	size_t ComputeSuperTriangle(MeshDataManipulator& dataManipulator, const ASinglyLinkedList<Vector2>& points)
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

		return dataManipulator.AddTriangle(
			Vector3(midWidth - 20.0f * biggerSide, midHeight - biggerSide, 0.0f), 
			Vector3(midWidth, midHeight + 20.0f * biggerSide, 0.0f), 
			Vector3(midWidth + 20.0f * biggerSide, midHeight - biggerSide, 0.0f));
	}

	bool IsEdgeSharedByOtherTriangles(const MeshDataManipulator& dataManipulator, 
		const ASinglyLinkedList<size_t>& triangles, size_t currTriangle, size_t edge)
	{
		for (size_t triangleID : triangles)
		{
			if (triangleID != currTriangle && dataManipulator.TriangleContainsEdge(triangleID, edge))
			{
				return true;
			}
		}
		return false;
	}

	void ConvertPointToVertexIndexRepresentation(ADynArr<size_t>& pointIDs,
		ADynArr<unsigned int>& indices, size_t point)
	{
		int index = pointIDs.Find(point);
		if (index == -1)
		{
			indices.Add(pointIDs.GetCount());
			pointIDs.Add(point);
		}
		else
		{
			indices.Add((unsigned int)index);
		}
	}

	MeshHandle Generate2DMesh(const MeshDataManipulator& dataManipulator,
		const ASinglyLinkedList<size_t>& triangulation)
	{
		ADynArr<size_t> pointIDs = ADynArr<size_t>(triangulation.GetCount());
		ADynArr<Vector3> meshVertices = ADynArr<Vector3>(triangulation.GetCount());
		ADynArr<unsigned int> meshIndices = ADynArr<unsigned int>(triangulation.GetCount());

		for (size_t triangleID : triangulation)
		{
			size_t p1 = dataManipulator.GetTrianglePoint1ID(triangleID);
			size_t p2 = dataManipulator.GetTrianglePoint2ID(triangleID);
			size_t p3 = dataManipulator.GetTrianglePoint3ID(triangleID);

			ConvertPointToVertexIndexRepresentation(pointIDs, meshIndices, 
				dataManipulator.GetTrianglePoint1ID(triangleID));
			ConvertPointToVertexIndexRepresentation(pointIDs, meshIndices,
				dataManipulator.GetTrianglePoint2ID(triangleID));
			ConvertPointToVertexIndexRepresentation(pointIDs, meshIndices,
				dataManipulator.GetTrianglePoint3ID(triangleID));
		}

		for (size_t id : pointIDs)
		{
			meshVertices.Add(dataManipulator.GetCoords(id));
		}

		ADynArr<Vector2> textureCoords = ADynArr<Vector2>(meshVertices.GetCount());
		ADynArr<Vector3> normals = ADynArr<Vector3>(meshVertices.GetCount());
		for (size_t i = 0; i < meshVertices.GetCount(); i++)
		{
			textureCoords.EmplaceBack(0.0f, 0.0f);
			normals.EmplaceBack(0.0f, 0.0f, -1.0f);
		}

		return ResourceHandler::CreateMesh(meshVertices, textureCoords, normals, meshIndices);
	}

	MeshHandle Tessellation::BoyerWatson(const ASinglyLinkedList<Vector2>& points)
	{
		MeshDataManipulator dataManipulator;
		size_t superTriangle = ComputeSuperTriangle(dataManipulator, points);
		ASinglyLinkedList<size_t> triangulation;
		triangulation.Add(superTriangle);

		for (const Vector2& point : points)
		{
			ASinglyLinkedList<size_t> badTriangles;

			// find all invalid triangles
			for (size_t& triangle : triangulation)
			{
				if (dataManipulator.PointIsInTriangleCircumsphere(triangle, point))
				{
					badTriangles.Add(triangle);
				}
			}

			//construct edge list of the polygon created by the invalid triangles
			ASinglyLinkedList<size_t> polygon;

			for (size_t& triangle : badTriangles)
			{
				size_t currEdge = dataManipulator.GetEdge1ID(triangle);
				if (!IsEdgeSharedByOtherTriangles(dataManipulator, badTriangles, triangle, currEdge))
				{
					polygon.Add(currEdge);
				}
				
				currEdge = dataManipulator.GetEdge2ID(triangle);
				if (!IsEdgeSharedByOtherTriangles(dataManipulator, badTriangles, triangle, currEdge))
				{
					polygon.Add(currEdge);
				}

				currEdge = dataManipulator.GetEdge3ID(triangle);
				if (!IsEdgeSharedByOtherTriangles(dataManipulator, badTriangles, triangle, currEdge))
				{
					polygon.Add(currEdge);
				}
			}

			// remove the invalid triangles from the triangulation
			for (size_t triangle : badTriangles)
			{
				triangulation.Remove(triangle);
			}

			// create and add triangles between the point of the point list and the edges of 
			// the polygon left from the invalid triangles
			for (size_t edge : polygon)
			{
				size_t p1 = dataManipulator.GetPoint1ID(edge);
				size_t p2 = dataManipulator.GetPoint2ID(edge);

				size_t id = dataManipulator.AddTriangle(dataManipulator.GetCoords(p1), 
					dataManipulator.GetCoords(p2), point);

				if (id != NullID)
				{
					triangulation.Add(id);
				}
			}
		}

		// remove any triangle that contains a point from the super triangle to clean up the triangulation
		{
			ASinglyLinkedList<size_t> toRemove;
			for (size_t triangle : triangulation)
			{
				if (dataManipulator.TrianglesSharePoint(superTriangle, triangle))
				{
					toRemove.Add(triangle);
				}
			}

			for (size_t triangle : toRemove)
			{
				triangulation.Remove(triangle);
			}
		}

		return Generate2DMesh(dataManipulator, triangulation);
	}
}