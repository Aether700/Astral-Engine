#include "aepch.h"
#include "Utils.h"
#include <cmath>

namespace AstralEngine
{
	float Math::Abs(float v)
	{
		AE_PROFILE_FUNCTION();
		if (v < 0.0f)
		{
			return -1.0f * v;
		}
		return v;
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

	float Math::Sin(float v)
	{
		AE_PROFILE_FUNCTION();
		return sin(v);
	}

	float Math::Cos(float v)
	{
		AE_PROFILE_FUNCTION();
		return cos(v);
	}

	float Math::Tan(float v)
	{
		AE_PROFILE_FUNCTION();
		return tan(v);
	}

	float Math::DegreeToRadiants(float degrees)
	{
		AE_PROFILE_FUNCTION();
		return degrees * 0.01745329251994329576923690768489f;
	}

	float Math::Sqrt(float v)
	{
		AE_PROFILE_FUNCTION();
		return sqrt(v);
	}

	float Math::Min(float f1, float f2) 
	{
		if (f1 < f2)
		{
			return f1;
		}

		return f2;
	}

	float Math::Max(float f1, float f2)
	{
		if (f1 > f2)
		{
			return f1;
		}

		return f2;
	}

	float Math::Floor(float f)
	{
		return (int)f;
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

}