#include "aepch.h"
#include "Utils.h"
#include <cmath>

namespace AstralEngine
{
	int Math::Abs(int v)
	{
		AE_PROFILE_FUNCTION();
		if (v < 0)
		{
			return -1 * v;
		}
		return v;
	}

	float Math::Abs(float v)
	{
		AE_PROFILE_FUNCTION();
		if (v < 0.0f)
		{
			return -1.0f * v;
		}
		return v;
	}

	double Math::Abs(double v)
	{
		AE_PROFILE_FUNCTION();
		if (v < 0.0)
		{
			return -1.0 * v;
		}
		return v;
	}

	float Math::Sin(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::sin(v);
	}

	float Math::ArcSin(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::asin(v);
	}

	float Math::Cos(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::cos(v);
	}

	float Math::ArcCos(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::acos(v);
	}

	float Math::Tan(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::tan(v);
	}

	float Math::ArcTan(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::atan(v);
	}

	float Math::ArcTan2(float y, float x)
	{
		AE_CORE_ASSERT(!(x == 0 && y == 0), "Arguments to ArcTan2 cannot be both 0.0f");
		if (x > 0.0f)
		{
			return ArcTan(y / x);
		}
		else if (y > 0.0f)
		{
			return ((float)Pi() / 2.0f) - ArcTan(x / y);
		}
		else if (y < 0.0f)
		{
			return -((float)Pi() / 2.0f) - ArcTan(x / y);
		}
		return ArcTan(y / x) + (float)Pi();
		//return atan2(y, x);
	}

	float Math::ArcTan2(Vector2 v)
	{
		return ArcTan2(v.y, v.x);
	}

	float Math::Power(float num, int power)
	{
		if (power == 1)
		{
			return num;
		}
		else if (power == 0)
		{
			return 1.0f;
		}
		else if (power < 0)
		{
			return 1.0f / Power(num, -power);
		}

		int halfPower = power / 2;
		if (power % 2 == 0)
		{
			return Power(num, halfPower) * Power(num, halfPower);
		}
		return num * Power(num, halfPower) * Power(num, halfPower);
	}

	float Math::Sqrt(float v)
	{
		AE_PROFILE_FUNCTION();
		return std::sqrt(v);
	}

	float Math::Log(float v)
	{
		return std::log(v);
	}

	float Math::CopySign(float magnitude, float sign)
	{
		if (sign == 0.0f)
		{
			return Abs(magnitude);
		}
		return Abs(magnitude) * Abs(sign) / sign;
	}

	long double Math::Pi()
	{
		return 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
	}

	float Math::DegreeToRadians(float degrees)
	{
		return degrees * 0.01745329251994329576923690768489f;
	}

	float Math::RadiansToDegree(float radians)
	{
		return radians * 57.295779513082320876798154814105f;
	}

	float Math::Lerp(float a, float b, float t)
	{
		if (t > 1.0f)
		{
			return b;
		}
		else if (t < 0.0f)
		{
			return a;
		}

		return a + t * (b - a);
	}

	float Math::Floor(float f)
	{
		return (float)((long)f);
	}

	Vector2 Math::Floor(Vector2 v)
	{
		return Vector2(Math::Floor(v.x), Math::Floor(v.y));
	}
	
	float Math::Ceiling(float f)
	{
		float floor = Floor(f);
		if (f - floor == 0)
		{
			return f;
		}
		return floor + 1.0f;
	}

	//provides a hashed gradient vector for the provided coordinates
	Vector2 GradientHash(int x, int y)
	{
		float k = 2920.0f * Math::Sin(x * 21942.f + y * 171324.f + 8912.f) 
			* Math::Cos(x * 23157.f * y * 217832.f + 9758.f);
		return Vector2(Math::Cos(k), Math::Sin(k));
	}

	//used by PerlinNoise, takes the grid coordinates as well as the pos coordinates
	float DotProdOfGridAndGradient(int gridX, int gridY, float posX, float posY)
	{
		Vector2 gradient = GradientHash(gridX, gridY);

		//coords of the distance vector
		float distX = posX - gridX;
		float distY = posY - gridY;

		//return dot product
		return distX * gradient.x + distY * gradient.y;
	}

	//used by the perlin noise function, smoothen the provided float
	float FadeFunction(float t)
	{
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	float Math::PerlinNoise(float x, float y)
	{
		//grid coords
		int x0 = (int)x;
		int x1 = x0 + 1;
		int y0 = (int)y;
		int y1 = y0 + 1;

		//used for interpolation
		float weightX = FadeFunction(x - (float)x0);
		float weightY = FadeFunction(y - (float)y0);

		float n0 = DotProdOfGridAndGradient(x0, y0, x, y);
		float n1 = DotProdOfGridAndGradient(x1, y0, x, y);
		float ix0 = Math::Lerp(n0, n1, weightX);
	
		n0 = DotProdOfGridAndGradient(x0, y1, x, y);
		n1 = DotProdOfGridAndGradient(x1, y1, x, y);
		float ix1 = Math::Lerp(n0, n1, weightX);

		return (Math::Lerp(ix0, ix1, weightY) + 1.0f) / 2.0f;
	}

	float Math::PerlinNoise(Vector2 pos)
	{
		return PerlinNoise(pos.x, pos.y);
	}

	float Math::BezierQuadratic(float p0, float p1, float p2, float t)
	{
		if (t >= 1.0f)
		{
			return p2;
		}
		else if (t <= 0.0f)
		{
			return p0;
		}
		//B(t) = p1 + (1 - t)^2 * (p0 - p1) + t^2 (p2 - p1)
		float oneMinusT = 1 - t;
		return p1 + oneMinusT * oneMinusT * (p0 - p1) + t * t * (p2 - p1);
	}

	bool Math::PointIsConvex(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		// the algorithm works by using a modified version of a formula to compute the area 
		// of a triangle and then using the sign of the result to determine the handedness 
		// of the points A and C in relation to B

		// the original area formula is Area = |( a.x(b.y-c.y) + b.x(c.y-a.y) + c.x(a.y-b.y) ) / 2|
		// the formula is obtained by drawing trapezoids below the triangle using the X axis 
		// as a base for the trapezoids and computing the area using the trapezoids
		// then using the fact that adding the length of two uneven sides of the trapeze 
		// * the height of the trapeze / 2 is the area of the trapeze we can get a formula and 
		// refactor it into the above formula

		// we drop the absolute value and the divide by 2 since we only care about the sign

		float modifiedAreaResult = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
		return modifiedAreaResult < 0;
	}

	bool Math::LineSegmentsCollide(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4)
	{
		// compute the distance from the point of intersection of the two lines equations of the edge
		// then check if both are <= 1 but >= 0 then we have a collision

		// we compute the distance from the intersection point by using 
		// the following parametric equations for both lines
		// L = p1 + t(p2 - p1) => (x, y) = ( x1 + t(x2 - x1), y1 + t(y2 - y1) )
		// the equation holds for 0 <= t <= 1 otherwise we get off of the segment

		// using parameters s and t one per line, 
		// the two lines intersect if they need to reach a common point (x0, y0) for parameters s0, t0 and 0 <= s0, t0 <= 1 
		// s0 and t0 are obtained by solving the linear system below:

		// s(x2 - x1) - t(x4 - x3) = x3 - x1
		// s(y2 - y1) - t(y4 - y3) = y3 - y1

		// the formula below is the result from solving such a linear system

		float t0 = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x))
			/ ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));

		float s0 = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x))
			/ ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));


		// is both t0 and s0 are between 0 and 1 then we have a collision between the two segments
		return t0 >= 0 && t0 <= 1 && s0 >= 0 && s0 <= 1;
	}

	bool Math::IsPointInShape(const ADoublyLinkedList<Vector2>& shapeContour, const Vector2& point)
	{
		// for every edge in the shape we do two checks which effectively scan the shape horizontally
		// one check to verify if the point's y "height" is in between the two points of the edge
		// the second check attempts to check if the edge is "near enough" to the point on the x axis
		// the value of "near enough" is computed in relation to the length of the edge and 
		// it's distance with the point

		bool isInside = false;
		auto it = shapeContour.begin();
		for (; it != shapeContour.end(); it++)
		{
			const Vector2& curr = *it;
			auto oldIt = it;
			it++;
			if (it == shapeContour.end())
			{
				it = shapeContour.begin();
			}
			const Vector2& next = *it;
			it = oldIt;

			// first check -> make sure the point is in between the two ends of the current edge
			if ((curr.y > point.y) != (next.y > point.y))
			{

				// second check, look if the edge is "near enough" to the point on the x axis
				if (point.x < (((next.x - curr.x) * (point.y - curr.y) / (next.y - curr.y)) + curr.x))
				{
					// if both checks have passed toggle isInside since we are doing horizontal scans of the shape
					isInside = !isInside;
				}
			}
		}
		return isInside;

		/*
		bool isInside = false;
		for (size_t i = 0; i < shapeContour.GetCount(); i++)
		{
			const Vector2& curr = shapeContour[i];
			size_t nextIndex = i + 1;
			if (nextIndex >= shapeContour.GetCount())
			{
				nextIndex = 0;
			}
			const Vector2& next = shapeContour[nextIndex];

			// first check -> make sure the point is in between the two ends of the current edge
			if ((curr.y > point.y) != (next.y > point.y))
			{

				// second check, look if the edge is "near enough" to the point on the x axis
				if (point.x < ( ((next.x - curr.x) * (point.y - curr.y) / (next.y - curr.y)) + curr.x ) )
				{
					// if both checks have passed toggle isInside since we are doing horizontal scans of the shape
					isInside = !isInside;
				}
			}
		}
		return isInside;
		*/
	}

	Vector3 Math::ComputeBarycentricCoords(const Vector3& p, const Vector3& t1, const Vector3& t2, const Vector3& t3)
	{
		// By definition Barycentric coords are defined by drawing an edge between the point P and 
		// each of the vertices of the triangle. Then, each barycentric coordinates is defined as follows:

		// X = TriangleArea(p, t2, t3) / TriangleArea(t1, t2, t3)
		// Y = TriangleArea(p, t1, t3) / TriangleArea(t1, t2, t3)
		// Z = TriangleArea(p, t1, t2) / TriangleArea(t1, t2, t3)

		// to efficiently compute these coordinates we use a different method based on Cramer's rule 
		// which provides us with a simple equation to solve a system of linear with as many equations 
		// as unknowns. Given that our problem can be represented as such a system we can simply implement 
		// this equation for an efficient way to compute the Barycentric coordinates of our point

		Vector3 v0 = t2 - t1;
		Vector3 v1 = t3 - t1;
		Vector3 v2 = p - t1;

		float d00 = Vector3::DotProduct(v0, v0);
		float d01 = Vector3::DotProduct(v0, v1);
		float d11 = Vector3::DotProduct(v1, v1);
		float d20 = Vector3::DotProduct(v2, v0);
		float d21 = Vector3::DotProduct(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		
		float coordX = (d11 * d20 - d01 * d21) / denom;
		float coordY = (d00 * d21 - d01 * d20) / denom;
		float coordZ = 1.0f - coordX - coordY;

		return { coordX, coordY, coordZ };
	}

	// Random ////////////////////////////////////////

	/*initializes the seed of the rand function using the standard srand function
	by passing it the current time (time(NULL) returns the current time)
	*/
	void Random::Init()
	{
		srand((unsigned int)time(nullptr));
	}

	//wrapper function for the c function rand
	int Random::GetInt() { return rand(); }

	/* takes the value returned by rand as a float and divides it by
		32000.0f (this specific value was obtained through trial and errors)
	*/
	float Random::GetFloat()
	{
		return (float)rand() / 32000.0f;
	}
}