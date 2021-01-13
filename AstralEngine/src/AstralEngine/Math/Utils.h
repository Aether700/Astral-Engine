#pragma once

namespace AstralEngine
{
	class Math
	{
	public:
		static float Abs(float v);
		static float Clamp(float value, float min, float max);
		static float Sin(float v);
		static float Cos(float v);
		static float Tan(float v);
		static float DegreeToRadiants(float v);
		static float Sqrt(float v);
		static float Min(float f1, float f2);
		static float Max(float f1, float f2);
		static float Floor(float f);
		static float Ceiling(float f);
	};
}