#include "aepch.h"
#include "Tessellation.h"

namespace AstralEngine
{
	class TriangleData;
	struct EdgeData;

	decide on design to store data from triangles, edges and points to better optimize algorithm

	struct PointData
	{
		Vector2 coords;
		size_t index;
	};

	// tries to create a point if none with the given coordinates exists already. returns the index of 
	// the newly created point or of the existing point with the provided coordinantes
	size_t CreatePoint(ADynArr<PointData>& points, const Vector2& point)
	{
		for (PointData& data : points)
		{
			if (data.coords == point)
			{
				return data.index;
			}
		}

		size_t index = points.GetCount();
		PointData& data = points.EmplaceBack();
		data.coords = point;
		data.index = index;
		return index;
	}

	void CreateTriangle(ADynArr<PointData>& points, ADynArr<EdgeData>& edges, ADynArr<TriangleData>& triangles, 
		const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		size_t point1 = CreatePoint(points, p1);
		size_t point2 = CreatePoint(points, p2);
		size_t point3 = CreatePoint(points, p3);


	}

	struct EdgeData
	{
		PointData* point1;
		PointData* point2;
		size_t index;
		ASinglyLinkedList<TriangleData*> connectedTriangles;

		EdgeData() { }
		EdgeData(PointData* p1, PointData* p2) : point1(p1), point2(p2) { }
	};

	class TriangleData
	{
	public:
		TriangleData(PointData* a, PointData* b, PointData* c)
			: m_pointA(a), m_pointB(b), m_pointC(c) 
		{
			ComputeCircumcircle();
		}

		PointData* GetPointA() const { return m_pointA; }
		PointData* GetPointB() const { return m_pointB; }
		PointData* GetPointC() const { return m_pointC; }
		size_t GetIndex() const { return m_index; }

		void SetIndex(size_t index) { m_index = index; };

		// checks if the provided point is one of the 3 points forming the triangle
		bool ContainsPoint(PointData* point) const
		{
			return m_pointA == point || m_pointB == point || m_pointC == point;
		}

		bool ContainsEdge(PointData* edgePoint1, PointData* edgePoint2) const
		{
			return (m_pointA == edgePoint1 && m_pointB == edgePoint2)
				|| (m_pointA == edgePoint1 && m_pointC == edgePoint2)
				|| (m_pointB == edgePoint1 && m_pointC == edgePoint2)
				// flip all points
				|| (m_pointA == edgePoint2 && m_pointB == edgePoint1)
				|| (m_pointA == edgePoint2 && m_pointC == edgePoint1)
				|| (m_pointB == edgePoint2 && m_pointC == edgePoint1);
		}

		// checks if the provided point is inside the circumcircle of this triangle
		bool IsInCircumcirle(Vector2 point)
		{
			return Vector2::SqrDistance(m_circumcircleCenter, point) <= m_circumcircleRadiusSqr;
		}

		bool operator==(const TriangleData& other) const
		{
			return m_pointA == other.m_pointA && m_pointB == other.m_pointB && m_pointC == other.m_pointC;
		}
		
		bool operator!=(const TriangleData& other) const
		{
			return !(*this == other);
		}

	private:
		void ComputeCircumcircle()
		{
			float sin2AngleA = Math::Sin(2.0f * Vector2::Angle(m_pointB->coords, m_pointC->coords));
			float sin2AngleB = Math::Sin(2.0f * Vector2::Angle(m_pointA->coords, m_pointC->coords));
			float sin2AngleC = Math::Sin(2.0f * Vector2::Angle(m_pointA->coords, m_pointB->coords));

			float sumOfSinAngles = sin2AngleA + sin2AngleB + sin2AngleC;

			m_circumcircleCenter = Vector2(
				(m_pointA->coords.x * sin2AngleA + m_pointB->coords.x * sin2AngleB + m_pointC->coords.x * sin2AngleC) / sumOfSinAngles,
				(m_pointA->coords.y * sin2AngleA + m_pointB->coords.y * sin2AngleB + m_pointC->coords.y * sin2AngleC) / sumOfSinAngles
			);

			m_circumcircleRadiusSqr = Vector2::SqrDistance(m_pointA->coords, m_circumcircleCenter);
		}

		PointData* m_pointA;
		PointData* m_pointB;
		PointData* m_pointC;

		size_t m_index;

		Vector2 m_circumcircleCenter; 
		float m_circumcircleRadiusSqr;
	};

	TriangleData ComputeSuperTriangle(const ASinglyLinkedList<Vector2>& points)
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

		return TriangleData(
			{ midWidth - 20.0f * biggerSide, midHeight - biggerSide }, 
			{ midWidth, midHeight + 20.0f * biggerSide }, 
			{ midWidth + 20.0f * biggerSide, midHeight - biggerSide });
	}

	// can be optimized by storing adjacent triangle/edges data 
	bool IsEdgeSharedByOtherTriangles(const ASinglyLinkedList<TriangleData> triangleList, 
		const TriangleData* currTriangle, const Vector2& edgePoint1, const Vector2& edgePoint2)
	{
		for (const TriangleData& triangle : triangleList)
		{
			if (&triangle != currTriangle)
			{
				if (triangle.ContainsEdge(edgePoint1, edgePoint2))
				{
					return true;
				}
			}
		}
		return false;
	}

	void Tessellation::BoyerWatson(const ASinglyLinkedList<Vector2>& points,
		ASinglyLinkedList<Vector2>& vertices, ASinglyLinkedList<unsigned int>& indices)
	{
		TriangleData superTriangle = ComputeSuperTriangle(points);
		ASinglyLinkedList<TriangleData> triangulation;
		triangulation.Add(superTriangle);

		for (const Vector2& point : points)
		{
			ASinglyLinkedList<TriangleData> badTriangles;

			// find all invalid triangles
			for (TriangleData& triangle : triangulation)
			{
				if (triangle.IsInCircumcirle(point))
				{
					badTriangles.Add(triangle);
				}
			}

			//construct edge list of the polygon created by the invalid triangles
			ASinglyLinkedList<EdgeData> polygon;

			for (TriangleData& triangle : badTriangles)
			{
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