#pragma once
#include "AstralEngine/Math/AMath.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace AstralEngine
{
	class Input
	{
		friend class Application;
	public:
		static bool GetKey(int keycode);
		static bool GetKey(KeyCode key);
		static bool GetAnyKey();
		

		static bool GetKeyDown(int keycode);
		static bool GetKeyDown(KeyCode key);
		static bool GetAnyKeyDown();
		
		
		static bool GetMouseButton(int mouseButton);
		static bool GetMouseButton(MouseButtonCode mouseButton);
		
		static bool GetMouseButtonDown(int mouseButton);
		static bool GetMouseButtonDown(MouseButtonCode mouseButton);
		
		static Vector2 GetMousePosition();
		static float GetMouseXPos();
		static float GetMouseYPos();

	private:
		static void OnUpdate();
		static bool OnKeyPressedEvent(KeyPressedEvent& keyPressed);
		static bool OnKeyReleasedEvent(KeyReleasedEvent& keyReleased);
		static bool OnMousePressedEvent(MouseButtonPressedEvent& mousePressed);
		static bool OnMouseReleasedEvent(MouseButtonReleasedEvent& mouseReleased);
	};

}