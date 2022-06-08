#pragma once
#include "Vectors/Vector2.h"

namespace AstralEngine
{
	class Math
	{
	public:
		static int Abs(int v);
		static float Abs(float v);
		static double Abs(double v);
		static int Clamp(int value, int min, int max);
		static float Clamp(float value, float min, float max);
		
		// Uses Radians
		static float Sin(float v);
		static float ArcSin(float v);
		static float Cos(float v);
		static float ArcCos(float v);
		static float Tan(float v);

		static float DegreeToRadiants(float degrees);
		static float RadiantsToDegree(float radians);

		static float ArcTan(float v);
		static float ArcTan2(float y, float x);
		static float ArcTan2(Vector2 v);

		static float Sqrt(float v);
		static float Log(float v);

		static int Min(int i1, int i2);
		static float Min(float f1, float f2);
		static int Max(int i1, int i2);
		static float Max(float f1, float f2);

		static float CopySign(float magnitude, float sign);

		static long double Pi();

		static float DegreeToRadiants(float degrees);
		static float RadiantsToDegree(float radians);
		
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
		static double Pi();
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