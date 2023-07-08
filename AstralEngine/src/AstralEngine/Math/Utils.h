#pragma once
#include "Vectors/Vector2.h"
#include "AstralEngine/Data Struct/ADynArr.h"

namespace AstralEngine
{
	class Math
	{
	public:
		static int Abs(int v);
		static float Abs(float v);
		static double Abs(double v);

		template<typename T>
		static T Clamp(T value, T min, T max)
		{
			if (value < min)
			{
				return min;
			}
			else if (value > max)
			{
				return max;
			}
			return value;
		}
		
		// Uses Radians
		static float Sin(float v);
		static float ArcSin(float v);
		static float Cos(float v);
		static float ArcCos(float v);
		static float Tan(float v);
		static float ArcTan(float v);
		static float ArcTan2(float y, float x);
		static float ArcTan2(Vector2 v);

		static float Power(float num, int power);
		static float Sqrt(float v);
		static float Log(float v);

		template<typename T>
		static T Min(T t1, T t2)
		{
			if (t1 < t2)
			{
				return t1;
			}
			return t2;
		}

		template<typename T>
		static T Max(T t1, T t2)
		{
			if (t1 > t2)
			{
				return t1;
			}
			return t2;
		}

		static float CopySign(float magnitude, float sign);

		static long double Pi();

		static float DegreeToRadians(float degrees);
		static float RadiansToDegree(float radians);
		
		/*linearly interpolates from a to b using t (t must be between 0 and 1)
			if t >= 0, returns a
			if t >= 1, returns b
			if t = 0.5, returns the midpoint of a and b
		*/
		static float Lerp(float a, float b, float t);

		static float Floor(float f);
		static Vector2 Floor(Vector2 v);
		static float Ceiling(float f);
		
		static float PerlinNoise(float x, float y);
		static float PerlinNoise(Vector2 pos);

		// computes the quadratic bezier curve given the provided parameters.
		// B(t) = p1 + (1 - t)^2 * (p0 - p1) + t^2 (p2 - p1) for any 0 <= t <= 1 
		static float BezierQuadratic(float p0, float p1, float p2, float t);

		// checks if the point b is convex in relation to the triangle a, b, c
		static bool PointIsConvex(const Vector2& a, const Vector2& b, const Vector2& c);

		// checks if the line segment p1 p2 intersects with the line segment p3 p4
		static bool LineSegmentsCollide(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4);
		
		// checks if the provided point is inside the provided shape
		static bool IsPointInShape(const ADynArr<Vector2>& shapeContour, const Vector2& point);

		// computes the Barycentric coords of the provided point p relative the the triangle made of the points t1, t2 and t3
		static Vector3 ComputeBarycentricCoords(const Vector3& p, const Vector3& t1, const Vector3& t2, const Vector3& t3);

	};

	/* Basic rudementary Random class to get random ints and floats
	*/
	class Random
	{
	public:
		/*	Initializes the Random class by setting the seed of the c
			function rand to the current time allowing pseudo random number generation

			Needs to be used at the start of the program to set the seed. Do not use more than once.
			only use when starting your program
		*/
		static void Init();

		/*	returns a random integer between 0 and RAND_MAX
			(a maximum value defined by c libraries which is at least 32767).
			Note that RAND_MAX cannot be returned

			returns: a random integer between 0 and RAND_MAX (RAND_MAX not included)
		*/
		static int GetInt();

		/* returns a random float number between 0 and 1 (one not included)

			returns: a random float number between 0 and 1 (one not included)
		*/
		static float GetFloat();

	};
}