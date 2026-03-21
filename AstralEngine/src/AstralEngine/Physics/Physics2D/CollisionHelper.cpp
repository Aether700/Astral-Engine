#include "aepch.h"
#include "CollisionHelper.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/ECS/Components.h"

namespace AstralEngine
{
	void RetrieveVertices(const BoxCollider2D& box, ADynArr<Vector2>& outVertices)
	{
		const Transform& t = box.GetTransform();
		Vector2 center = t.GetLocalPosition() + box.GetOffset();
		float halfWidth = box.GetWidth() / 2.0f;
		float halfHeight = box.GetHeight() / 2.0f;

		Vector2 min = Vector2(center.x - halfWidth, center.y - halfHeight);
		Vector2 max = Vector2(center.x + halfWidth, center.y + halfHeight);

		outVertices.Add(t.GetRotation() * min);
		outVertices.Add(t.GetRotation() * Vector2(max.x, min.y));
		outVertices.Add(t.GetRotation() * max);
		outVertices.Add(t.GetRotation() * Vector2(min.x, max.y));
	}

	void ProjectOnAxis(const ADynArr<Vector2>& polygon, const Vector2& axis, float& outMin, float& outMax)
	{
		outMin = FLT_MAX;
		outMax = FLT_MIN;

		for (const Vector2& vertex : polygon)
		{
			float projection = Vector2::DotProduct(axis, vertex);

			if (projection < outMin)
			{
				outMin = projection;
			}

			if (projection > outMax)
			{
				outMax = projection;
			}
		}
	}

	/// <summary>
	/// Finds the point closest on the line drawn by v1 and v2 from the provided point
	/// </summary>
	/// <param name="point">The point to which the positions on the lines are compared</param>
	/// <param name="v1">The start of the line</param>
	/// <param name="v2">The end of the line</param>
	/// <returns>The closest point to the point provided that is on the line</returns>
	Vector2 ClosestPointOnLine(const Vector2& point, const Vector2& v1, const Vector2& v2)
	{
		Vector2 line = v2 - v1;
		Vector2 v1ToPoint = point - v1;

		// divide by magnitude square because we divide once by magnitude of the line to 
		// remove the scaling of the line in the dot product itself and then once more to 
		// get the unit vector in the direction of the line
		//
		// this is because the dot product can be defined as a * b = |a| |b| cos(theta) which 
		// scales the value of the vector by both a and b but we only care about the length of a
		// so we need to remove the extra |b| and then another time for the normal unit vector computation
		float proj = Vector2::DotProduct(line, v1ToPoint) / line.SqrMagnitude();

		if (proj <= 0.0f)
		{
			return v1;
		}
		else if (proj >= 1.0f)
		{
			return v2;
		}
		return v1 + (proj * line);
	}

	void CheckCollisionPointsFromPolygon(const ADynArr<Vector2>& verticesPolygon1,
		const ADynArr<Vector2>& verticesPolygon2, ADynArr<Vector2>& outCollisionPoints, float& closestDistance) 
	{
		const float epsilon = 0.000001f;

		for (size_t i = 0; i < verticesPolygon1.GetCount(); i++)
		{
			const Vector2& a = verticesPolygon1[i];
			const Vector2& b = verticesPolygon1[(i + 1) % verticesPolygon1.GetCount()];

			for (const Vector2& point : verticesPolygon2)
			{
				Vector2 closestPoint = ClosestPointOnLine(point, a, b);
				float currDistance = Vector2::SqrDistance(closestPoint, point);
				if (Math::Abs(currDistance - closestDistance) < epsilon)
				{
					outCollisionPoints.Add(point);
				}
				else if (currDistance < closestDistance)
				{
					closestDistance = currDistance;
					outCollisionPoints.Clear();
					outCollisionPoints.Add(point);
				}
			}
		}
	}

	void FindCollisionPoints(const ADynArr<Vector2>& verticesPolygon1, 
		const ADynArr<Vector2>& verticesPolygon2, ADynArr<Vector2>& outCollisionPoints)
	{
		float closestDistance = FLT_MAX;
		CheckCollisionPointsFromPolygon(verticesPolygon1, verticesPolygon2, outCollisionPoints, closestDistance);
		CheckCollisionPointsFromPolygon(verticesPolygon2, verticesPolygon1, outCollisionPoints, closestDistance);
	}

	// applies the separating axis theorem to the normals of the first polygon and tries to 
	// find an axis that seperates both shapes
	// returns true if a seperating axis was found, false otherwise.
	// Needs to be called twice to check from both polygon's perspective 
	// to make sure if there is a seperating axis or not
	bool HasSeperatingAxis(const ADynArr<Vector2>& verticesPolygon1, const ADynArr<Vector2>& verticesPolygon2, 
		Vector2& outNormal, float& outDepth)
	{
		for (int i = 0; i < verticesPolygon1.GetCount(); i++)
		{
			const Vector2& p1 = verticesPolygon1[i];
			const Vector2& p2 = verticesPolygon1[(i + 1) % verticesPolygon1.GetCount()];

			Vector2 edge = p2 - p1;
			Vector2 normal = Vector2(edge.y, -edge.x);
			normal.Normalize();

			float v1Min;
			float v1Max;
			float v2Min;
			float v2Max;

			ProjectOnAxis(verticesPolygon1, normal, v1Min, v1Max);
			ProjectOnAxis(verticesPolygon2, normal, v2Min, v2Max);

			if (v1Min >= v2Max|| v2Min >= v1Max)
			{
				return true;
			}

			float currDepth = v1Min - v2Max;
			if (outDepth > currDepth)
			{
				outDepth = currDepth;
				outNormal = normal;
			}
		}

		return false;
	}

	bool PolygonToPolygonCollision(const ADynArr<Vector2>& verticesP1, const ADynArr<Vector2>& verticesP2, 
		Vector2& outNormal, float& outDepth)
	{
		outDepth = FLT_MAX;

		if (HasSeperatingAxis(verticesP1, verticesP2, outNormal, outDepth))
		{
			return false;
		}

		return !HasSeperatingAxis(verticesP2, verticesP1, outNormal, outDepth);
	}

	bool CollisionHelper::BoxToBoxCollision(const BoxCollider2D& b1, const BoxCollider2D& b2, 
		AReference<Collision2DInfo>& outCollisionInfo)
	{
		ADynArr<Vector2> b1Vertices = ADynArr<Vector2>(4);
		ADynArr<Vector2> b2Vertices = ADynArr<Vector2>(4);

		RetrieveVertices(b1, b1Vertices);
		RetrieveVertices(b2, b2Vertices);

		Vector2 normal;
		float depth;

		// might be able to optimize for boxes
		// Can definitively optimize for Axis Aligned Bounding Boxes (if rotation is 0)
		if (!PolygonToPolygonCollision(b1Vertices, b2Vertices, normal, depth))
		{
			outCollisionInfo = nullptr;
			return false;
		}
		
		ADynArr<Vector2> collisionPoints;
		FindCollisionPoints(b1Vertices, b2Vertices, collisionPoints);

		outCollisionInfo = AReference<Collision2DInfo>::Create(normal, depth, std::move(collisionPoints));
		return true;
	}

	bool CollisionHelper::BoxToBoxCollision(const BoxCollider2D& b1, const BoxCollider2D& b2)
	{
		AReference<Collision2DInfo> collision;
		return BoxToBoxCollision(b1, b2, collision);
	}
}