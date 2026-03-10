#include "aepch.h"
#include "CollisionHelper.h"
#include "AstralEngine/Data Struct/ADynArr.h"

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

	// applies the separating axis theorem to the normals of the first polygon and tries to 
	// find an axis that seperates both shapes
	// returns true if a seperating axis was found, false otherwise.
	// Needs to be called twice to check from both polygon's perspective 
	// to make sure if there is a seperating axis or not
	bool HasSeperatingAxis(const ADynArr<Vector2>& v1, const ADynArr<Vector2>& v2)
	{
		for (int i = 0; i < v1.GetCount(); i++)
		{
			const Vector2& p1 = v1[i];
			const Vector2& p2 = v1[(i + 1) % v1.GetCount()];

			Vector2 edge = p2 - p1;
			Vector2 normal = Vector2(edge.y, -edge.x);

			float v1Min;
			float v1Max;
			float v2Min;
			float v2Max;

			is slightly off in some cases but might be due to the 
				precision double check and try to fix if possible

			ProjectOnAxis(v1, normal, v1Min, v1Max);
			ProjectOnAxis(v2, normal, v2Min, v2Max);

			if (v1Min >= v2Max|| v2Min >= v1Max)
			{
				return true;
			}
		}

		return false;
	}

	bool PolygonToPolygonCollision(const ADynArr<Vector2>& verticesP1, const ADynArr<Vector2>& verticesP2)
	{
		if (HasSeperatingAxis(verticesP1, verticesP2))
		{
			return false;
		}

		return !HasSeperatingAxis(verticesP2, verticesP1);
	}

	bool CollisionHelper::BoxToBoxCollision(const BoxCollider2D& b1, const BoxCollider2D& b2)
	{
		ADynArr<Vector2> b1Vertices = ADynArr<Vector2>(4);
		ADynArr<Vector2> b2Vertices = ADynArr<Vector2>(4);

		RetrieveVertices(b1, b1Vertices);
		RetrieveVertices(b2, b2Vertices);

		// might be able to optimize for boxes
		// Can definitively optimize for Axis Aligned Bounding Boxes (if rotation is 0)
		return PolygonToPolygonCollision(b1Vertices, b2Vertices);
	}
}