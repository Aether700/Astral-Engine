#pragma once

#define AE_MOUSE_BUTTON_1         0
#define AE_MOUSE_BUTTON_2         1
#define AE_MOUSE_BUTTON_3         2
#define AE_MOUSE_BUTTON_4         3
#define AE_MOUSE_BUTTON_5         4
#define AE_MOUSE_BUTTON_6         5
#define AE_MOUSE_BUTTON_7         6
#define AE_MOUSE_BUTTON_8         7
#define AE_MOUSE_BUTTON_LEFT      AE_MOUSE_BUTTON_1
#define AE_MOUSE_BUTTON_RIGHT     AE_MOUSE_BUTTON_2
#define AE_MOUSE_BUTTON_MIDDLE    AE_MOUSE_BUTTON_3

namespace AstralEngine
{
	enum class MouseButtonCode
	{
		B1 = AE_MOUSE_BUTTON_1,
		B2 = AE_MOUSE_BUTTON_2,
		B3 = AE_MOUSE_BUTTON_3,
		B4 = AE_MOUSE_BUTTON_4,
		B5 = AE_MOUSE_BUTTON_5,
		B6 = AE_MOUSE_BUTTON_6,
		B7 = AE_MOUSE_BUTTON_7,
		B8 = AE_MOUSE_BUTTON_8,
		Left = AE_MOUSE_BUTTON_LEFT,
		Right = AE_MOUSE_BUTTON_RIGHT,
		Middle = AE_MOUSE_BUTTON_MIDDLE,
		Count = 12
	};
}

namespace std
{
	template<>
	struct hash<AstralEngine::MouseButtonCode>
	{
		size_t operator()(const AstralEngine::MouseButtonCode button) const
		{
			return (size_t)button;
		}
	};
}