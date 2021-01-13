#pragma once
#include "AstralEngine/Math/AMath.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace AstralEngine
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(int mouseButton);
		static bool IsMouseButtonPressed(MouseButtonCode mouseButton);
		static Vector2 GetMousePosition();
		static float GetMouseXPos();
		static float GetMouseYPos();
	};

}