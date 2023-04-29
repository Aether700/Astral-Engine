#include "aepch.h"
#include "Tessellation.h"

namespace AstralEngine
{
	Vector3Int GetEarIndices(size_t listSize, size_t tipIndex)
	{
		size_t prevIndex = tipIndex == 0 ? listSize - 1 : tipIndex - 1;
		size_t nextIndex = tipIndex == listSize - 1 ? 0 : tipIndex + 1;
		return Vector3Int((int)prevIndex, (int)tipIndex, (int)nextIndex);
	}

	bool IsAnEar(const ADoublyLinkedList<Vector2>& points, const Vector3Int& ear)
	{
		MeshDataManipulator tempMesh;
		tempMesh.AddTriangle(points[ear.x], points[ear.y], points[ear.z]);
		size_t triangleIndex = 0;
		for (const Vector2& currPoint : points)
		{
			if (triangleIndex != ear.x && triangleIndex != ear.y && triangleIndex != ear.z)
			{
				if (tempMesh.PointIsInsideTriangle(0, currPoint))
				{
					return false;
				}
			}
			triangleIndex++;
		}
		return true;
	}

	// returns the indices of the points forming the ear stored in a Vector3Int with the tip of the 
	// ear being stored in the y component of the vector. returns Vector3Int::Zero if no ear is found
	Vector3Int FindEar(const ADoublyLinkedList<Vector2>& points)
	{
		for (size_t i = 0; i < points.GetCount(); i++)
		{
			Vector3Int potentialEar = GetEarIndices(points.GetCount(), i);
			if (IsAnEar(points, potentialEar))
			{
				return potentialEar;
			}
		}

		return Vector3Int::Zero();
	}

	size_t ComputeSuperTriangle(MeshDataManipulator& dataManipulator, const ADynArr<Vector2>& points)
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

	MeshHandle Tessellation::BoyerWatson(const ADynArr<Vector2>& points)
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

	MeshHandle Tessellation::EarClipping(const ASinglyLinkedList<ADynArr<Vector2>>& points)
	{
		// for now assume we only have one ring of points
		ADoublyLinkedList<ADoublyLinkedList<Vector2>> copyPoints;
		for (auto& pointRing : points)
		{
			ADoublyLinkedList<Vector2> currRing;
			for (auto& point : pointRing)
			{
				currRing.AddLast(point);
			}
			copyPoints.AddLast(currRing);
		}

		MeshDataManipulator mesh;
		EarClippingImpl(copyPoints[0], mesh);
		
		return mesh.Generate2DMesh();
	}

	void Tessellation::EarClippingImpl(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh)
	{
		if (points.GetCount() < 3)
		{
			return;
		}
		Vector3Int ear = FindEar(points);
		currMesh.AddTriangle(points[ear.x], points[ear.y], points[ear.z]);
		points.RemoveAt(ear.y); // remove tip
		EarClippingImpl(points, currMesh);
	}
}