#include "aepch.h"
#include "Tessellation.h"

namespace AstralEngine
{
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

	MeshHandle Tessellation::EarClipping(const ADoublyLinkedList<ADynArr<Vector2>>& points,
		TessellationWindingOrder windingOrder)
	{
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
		ADoublyLinkedList<Vector2> finalPolygonRing;
		finalPolygonRing = std::move(copyPoints[0]);
		copyPoints.RemoveAt(0);

		while (!copyPoints.IsEmpty())
		{
			auto nextInnerPolygon = FindInnerPolygonWithRightMostVertex(copyPoints);
			finalPolygonRing = BuildBridge(*nextInnerPolygon, finalPolygonRing);
			copyPoints.Remove(nextInnerPolygon);
		}

		if (!ClipEars(finalPolygonRing, mesh, windingOrder))
		{
			return NullHandle;
		}

		return mesh.Generate2DMesh();
	}

	Vector3Int Tessellation::GetEarIndices(size_t listSize, size_t tipIndex, TessellationWindingOrder windingOrder)
	{
		size_t prevIndex = tipIndex == 0 ? listSize - 1 : tipIndex - 1;
		size_t nextIndex = tipIndex == listSize - 1 ? 0 : tipIndex + 1;

		// adjust the winding order by changing the order of the indices around the tip of the ear
		if (windingOrder == TessellationWindingOrder::ClockWise)
		{
			return Vector3Int((int)prevIndex, (int)tipIndex, (int)nextIndex);
		}
		return Vector3Int((int)nextIndex, (int)tipIndex, (int)prevIndex);
	}

	bool Tessellation::IsAnEar(const ADoublyLinkedList<Vector2>& points, const Vector3Int& ear)
	{
		const Vector2& earPoint1 = points[ear.x];
		const Vector2& earPoint2 = points[ear.y];
		const Vector2& earPoint3 = points[ear.z];

		// make sure the point is convex
		if (!Math::PointIsConvex(earPoint1, earPoint2, earPoint3))
		{
			return false;
		}


		// make sure none of the other points are in the triangle of the ear
		MeshDataManipulator tempMesh;
		tempMesh.AddTriangle(earPoint1, earPoint2, earPoint3);
		for (const Vector2& currPoint : points)
		{
			if (currPoint != earPoint1 && currPoint != earPoint2 && currPoint != earPoint3)
			{
				if (tempMesh.PointIsInsideTriangle(0, currPoint))
				{
					return false;
				}
			}
		}
		return true;
	}

	// returns the indices of the points forming the ear stored in a Vector3Int with the tip of the 
	// ear being stored in the y component of the vector. returns Vector3Int::Zero if no ear is found
	Vector3Int Tessellation::FindEar(const ADoublyLinkedList<Vector2>& points, TessellationWindingOrder windingOrder)
	{
		for (size_t i = 0; i < points.GetCount(); i++)
		{
			Vector3Int potentialEar = GetEarIndices(points.GetCount(), i, windingOrder);
			if (IsAnEar(points, potentialEar))
			{
				return potentialEar;
			}
		}

		return Vector3Int::Zero();
	}

	ADoublyLinkedListIterator<ADoublyLinkedList<Vector2>>
		Tessellation::FindInnerPolygonWithRightMostVertex(ADoublyLinkedList<ADoublyLinkedList<Vector2>>& innerPolygonList)
	{
		AE_CORE_ASSERT(!innerPolygonList.IsEmpty(), "");

		auto innerPolygonToReturn = innerPolygonList.begin();
		if (innerPolygonList.GetCount() > 1)
		{
			float maxXCoord = (*(*innerPolygonToReturn).begin()).x;

			// find maxXCoord for the current inner polygon
			for (Vector2& point : *innerPolygonToReturn)
			{
				if (point.x > maxXCoord)
				{
					maxXCoord = point.x;
				}
			}

			// check every inner polygon
			auto it = innerPolygonList.begin();
			it++;
			for (; it != innerPolygonList.end(); it++)
			{
				for (Vector2& point : *it)
				{
					if (point.x > maxXCoord)
					{
						maxXCoord = point.x;
						innerPolygonToReturn = it;
					}
				}
			}
		}

		return innerPolygonToReturn;
	}

	bool Tessellation::EdgeBlocksVisibility(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4)
	{
		// ignore collisions at the very end of the line segments
		if (p1 == p3 || p1 == p4 || p2 == p3 || p2 == p4)
		{
			return false;
		}

		return Math::LineSegmentsCollide(p1, p2, p3, p4);
	}

	bool Tessellation::IsValidBridgePair(ADoublyLinkedList<Vector2>::AIterator& p1, 
		ADoublyLinkedList<Vector2>::AIterator& p2, ADoublyLinkedList<Vector2>& ring1, ADoublyLinkedList<Vector2>& ring2)
	{
		// check for edges blocking visibility in ring 1
		auto it = ring1.begin();
		if (it == p1)
		{
			it++;
		}
		Vector2 lastPoint = *it;
		it++;

		for (; it != ring1.end(); it++)
		{
			if (it != p1)
			{
				if (EdgeBlocksVisibility(*p1, *p2, lastPoint, *it))
				{
					return false;
				}
				lastPoint = *it;
			}
		}
		
		if (EdgeBlocksVisibility(*p1, *p2, lastPoint, *ring1.begin()))
		{
			return false;
		}

		// check for edges blocking visibility in ring 2
		it = ring2.begin();
		if (it == p2)
		{
			it++;
		}
		lastPoint = *it;
		it++;

		for (; it != ring2.end(); it++)
		{
			if (it != p2)
			{
				if (EdgeBlocksVisibility(*p1, *p2, lastPoint, *it))
				{
					return false;
				}
				lastPoint = *it;
			}
		}

		if (EdgeBlocksVisibility(*p1, *p2, lastPoint, *ring2.begin()))
		{
			return false;
		}

		return true;
	}

	ADoublyLinkedList<Vector2> Tessellation::BuildBridge(ADoublyLinkedList<Vector2>& innerPolygon, 
		ADoublyLinkedList<Vector2>& outerPolygon)
	{
		// find 2 vertices that form the bridge then combine them into a single ring that gets returned
		AE_CORE_ASSERT(!innerPolygon.IsEmpty() && !outerPolygon.IsEmpty(), "");
		
		// first find point in the innerPolygon with greatest x component
		ADoublyLinkedList<Vector2>::AIterator bridgePointA = innerPolygon.begin();
		for (auto it = innerPolygon.begin(); it != innerPolygon.end(); it++)
		{
			if ((*it).x > (*bridgePointA).x) 
			{
				bridgePointA = it;
			}
		}

		the B character cannot find a bridge check why

		ADoublyLinkedList<Vector2>::AIterator bridgePointB = outerPolygon.end();
		// find a point on outerPolygon which can form a bridge with previously found point
		for (auto it = outerPolygon.begin(); it != outerPolygon.end(); it++)
		{
			if (IsValidBridgePair(bridgePointA, it, innerPolygon, outerPolygon))
			{
				bridgePointB = it;
				break;
			}
		}

		AE_CORE_ASSERT(bridgePointB != outerPolygon.end(), "");

		// let a be the bridge vertex in innerPolygon and b be bridge vertex in outerPolygon and 
		// x be first vertex in innerPolygon build the following ring 
		// {x, ...in innerPolygon..., a, b, ...in outerPolygon..., b, a, ...in innerPolygon, x}
		ADoublyLinkedList<Vector2> resultRing;
		
		// do first half of innerPolygon
		for (auto it = innerPolygon.begin(); it != bridgePointA; it++)
		{
			resultRing.AddLast(*it);
		}
		resultRing.AddLast(*bridgePointA);

		// do second half of outerPolygon
		for (auto it = bridgePointB; it != outerPolygon.end(); it++)
		{
			resultRing.AddLast(*it);
		}
		// wrap back around and do first half of outerPolygon
		for (auto it = outerPolygon.begin(); it != bridgePointB; it++)
		{
			resultRing.AddLast(*it);
		}
		resultRing.AddLast(*bridgePointB);

		// wrap back around and do second half of innerPolygon
		for (auto it = bridgePointA; it != innerPolygon.end(); it++)
		{
			resultRing.AddLast(*it);
		}

		return resultRing;
	}

	bool Tessellation::ClipEars(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh, 
		TessellationWindingOrder windingOrder)
	{
		if (points.GetCount() < 3)
		{
			return true;
		}
		Vector3Int ear = FindEar(points, windingOrder);
		// temp
		if (ear == Vector3Int::Zero())
		{
			AE_CORE_WARN("Could not find next ear:");
			for (auto& coords : points)
			{
				AE_CORE_INFO("%f, %f", coords.x, coords.y);
			}
		}
		else
		{
			AE_CORE_WARN("Ear found:");
			AE_CORE_INFO("%f, %f", points[ear.x].x, points[ear.x].y);
			AE_CORE_INFO("%f, %f", points[ear.y].x, points[ear.y].y);
			AE_CORE_INFO("%f, %f", points[ear.z].x, points[ear.z].y);
		}
		///

		//algorithm fails with A character->cannot find ear, check why

		if (ear == Vector3Int::Zero())
		{
			return false;
		}

		currMesh.AddTriangle(points[ear.x], points[ear.y], points[ear.z]);
		points.RemoveAt(ear.y); // remove tip
		return ClipEars(points, currMesh, windingOrder);
	}
}