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
		// problem with current implementation, it flags the hole in the middle as an ear

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
		// for now assume we have 2 rings one for the polygone one for the hole. The polygon ring is the first list
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
		if (copyPoints.GetCount() > 1)
		{
			// build finalPolygonRing here (for now only handles 2 ring case)
			finalPolygonRing = BuildBridge(copyPoints[0], copyPoints[1]);
		}
		else
		{
			finalPolygonRing = std::move(copyPoints[0]);
		}

		ClipEars(finalPolygonRing, mesh);
		
		return mesh.Generate2DMesh();
	}

	bool Tessellation::EdgeBlocksVisibility(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4)
	{
		// compute the distance from the point of intersection of the two lines equations of the edge
		// then check if both are <= 1 but >= 0 then we have a collision

		// the exact formula is obtained as follows
		// let a line L be perpendicular to the edge p3 and p4 going through p1. then, 
		// let P be the point on L closest to the point of intersection (p-int) Similarly, 
		// let Q be the point on L closest to p2. At this point the line P, p-int and Q, p2 
		// are parallel and they are both perpendicular to L

		// Now we have 2 similar triangles: p1, P, p-int and p1, Q, p2. the distance from p1 to P is 
		// the projection of p1 to p-int on L and similarly p1, Q is the projection of p1 to p2 on L. 
		// However, p1 to P is also the projection of p1 to p3 on L (assuming p3 is closer to L than p4 for simplicity)
		// Therefore, we can use the following formulas:

		// p1 to P = |(p2 - p1) * V | / Magnitude(V)
		// p1 to Q = |(p3-p1) * V|/Magnitude(V) 
		// where V is the vector p4-p3 and * is the dot product

		// we can then take the ratio of the two lengths and drop the denominator and the absolute sign obtaining
		// ( (p2 - p1) * V ) / ( (p3-p1) * V ) or
		// ( (p2 - p1) * (p4 - p3) ) / ( (p3-p1) * (p4 - p3) )
		
		/*
		// my code
		Vector2 vA = p4 - p3;
		Vector2 vB = p2 - p1;

		float uA2 = Vector2::DotProduct(p2 - p1, vA) / Vector2::DotProduct(p3 - p1, vA);
		float uB2 = Vector2::DotProduct(p4 - p3, vB) / Vector2::DotProduct(p1 - p3, vB);
		*/

		not finished going through and making sure it works/is understood

		// from website
		float uA = ((p4.x - p3.x)*(p1.y - p3.y) - (p4.y - p3.y)*(p1.x - p3.x)) 
			/ ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));

		float uB = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) 
			/ ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));


		// is both uA and uB are between 0 and 1 then we have a collision 
		// between the two segments and the visibility is blocked
		return uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1;
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

	ADoublyLinkedList<Vector2> Tessellation::BuildBridge(ADoublyLinkedList<Vector2>& ring1, ADoublyLinkedList<Vector2>& ring2)
	{
		// find 2 vertices that form the bridge then combine them into a single ring that gets returned
		AE_CORE_ASSERT(!ring1.IsEmpty() && !ring2.IsEmpty(), "");
		
		// first find point in r1 with greatest x component
		ADoublyLinkedList<Vector2>::AIterator bridgePointA = ring1.begin();
		for (auto it = ring1.begin(); it != ring1.end(); it++)
		{
			if ((*it).x > (*bridgePointA).x) 
			{
				bridgePointA = it;
			}
		}

		ADoublyLinkedList<Vector2>::AIterator bridgePointB = ring2.end();
		// find a point on r2 which can form a bridge with previously found point
		for (auto it = ring2.begin(); it != ring2.end(); it++) 
		{
			if (IsValidBridgePair(bridgePointA, it, ring1, ring2)) 
			{
				bridgePointB = it;
				break;
			}
		}

		AE_CORE_ASSERT(bridgePointB != ring2.end(), "");

		// let a be the bridge vertex in r1 and b be bridge vertex in r2 and x be first vertex in r1
		// build the following ring {x, ...in r1..., a, b, ...in r2..., b, a, ...in r1, x}
		ADoublyLinkedList<Vector2> resultRing;
		
		for (auto it = ring1.begin(); it != bridgePointA; it++)
		{
			resultRing.AddLast(*it);
		}
		resultRing.AddLast(*bridgePointA);

		// do second half of ring2
		for (auto it = bridgePointB; it != ring2.end(); it++)
		{
			resultRing.AddLast(*it);
		}
		// wrap back around and do first half of ring2
		for (auto it = ring2.begin(); it != bridgePointB; it++)
		{
			resultRing.AddLast(*it);
		}
		resultRing.AddLast(*bridgePointB);

		for (auto it = bridgePointA; it != ring1.end(); it++)
		{
			resultRing.AddLast(*it);
		}

		return resultRing;
	}

	void Tessellation::ClipEars(ADoublyLinkedList<Vector2>& points, MeshDataManipulator& currMesh)
	{
		if (points.GetCount() < 3)
		{
			return;
		}
		Vector3Int ear = FindEar(points);
		currMesh.AddTriangle(points[ear.x], points[ear.y], points[ear.z]);
		points.RemoveAt(ear.y); // remove tip
		ClipEars(points, currMesh);
	}
}