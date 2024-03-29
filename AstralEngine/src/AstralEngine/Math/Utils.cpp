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

	int Math::Clamp(int value, int min, int max)
	{
		AE_PROFILE_FUNCTION();
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

	float Math::Clamp(float value, float min, float max)
	{
		AE_PROFILE_FUNCTION();
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

	double Math::Clamp(double value, double min, double max)
	{
		AE_PROFILE_FUNCTION();
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

	float Math::DegreeToRadians(float degrees)
	{
		return degrees * 0.01745329251994329576923690768489f;
	}

	float Math::RadiansToDegree(float radians)
	{
		return radians * 57.295779513082320876798154814105f;
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