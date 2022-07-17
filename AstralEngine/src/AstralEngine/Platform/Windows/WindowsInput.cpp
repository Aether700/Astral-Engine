#include "aepch.h"

#ifdef AE_PLATFORM_WINDOWS

#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Application.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	struct InputState
	{
		bool currentlyDown = false;
		bool previouslyDown = false;
	};

	Keyboard Input::s_keyboard;
	Mouse Input::s_mouse;

	static AUnorderedMap<int, InputState> s_keyStates;
	static ADynArr<int> s_keysToUpdate;
	static bool s_isKeyPressed = false; // marks if a key was pressed, not repeated, this frame
	static int s_numKeyDown = 0; 

	static AUnorderedMap<int, InputState> s_mouseStates;
	static ADynArr<int> s_mouseToUpdate(3);
	
	/*
	bool Input::GetKey(int keycode)
	{
		//GLFWwindow* window = (GLFWwindow*)Application::GetWindow()->GetNativeWindow();
		//int status = glfwGetKey(window, keycode);
		//return status == GLFW_PRESS || status == GLFW_REPEAT;
		//return s_keyStates[keycode].currentlyDown;
		return GetKey((KeyCode)keycode);
	}
	*/

	bool Input::GetKey(KeyCode key)
	{
		return s_keyboard.GetKey(key);
	}

	bool Input::GetAnyKey()
	{ 
		//return s_numKeyDown > 0;
		return s_keyboard.GetAnyKey();
	}
	/*
	bool Input::GetKeyDown(int keycode)
	{
		/*
		if (!s_keyStates.ContainsKey(keycode))
		{
			return false;
		}
		return s_keyStates[keycode].currentlyDown && !s_keyStates[keycode].previouslyDown;
		/
		return GetKeyDown((KeyCode)keycode);
	}
	*/

	bool Input::GetKeyDown(KeyCode key)
	{
		return s_keyboard.GetKeyDown(key);
	}

	bool Input::GetAnyKeyDown()
	{
		//return s_isKeyPressed;
		return s_keyboard.GetAnyKeyDown();
	}

	/*
	bool Input::GetMouseButton(int mouseButton)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetWindow()->GetNativeWindow();
		int status = glfwGetMouseButton(window, mouseButton);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}
	*/
	
	bool Input::GetMouseButton(MouseButtonCode mouseButton)
	{
		return s_mouse.GetButton(mouseButton);
	}

	/*
	bool Input::GetMouseButtonDown(int mouseButton)
	{
		if (!s_mouseStates.ContainsKey(mouseButton))
		{
			return false;
		}
		return s_mouseStates[mouseButton].currentlyDown 
			&& !s_mouseStates[mouseButton].previouslyDown;
	}
	*/


	bool Input::GetMouseButtonDown(MouseButtonCode mouseButton)
	{
		return s_mouse.GetButtonDown(mouseButton);
	}

	Vector2Int Input::GetMousePosition() 
	{
		return s_mouse.GetMousePos();
	}
	
	int Input::GetMouseXPos() 
	{
		return s_mouse.GetMousePosX();
	}

	int Input::GetMouseYPos() 
	{
		return s_mouse.GetMousePosY();
	}

	void Input::OnUpdate()
	{
		/*
		if (!s_keysToUpdate.IsEmpty())
		{
			for (int key : s_keysToUpdate)
			{
				s_keyStates[key].previouslyDown = s_keyStates[key].currentlyDown;
			}

			s_keysToUpdate.Clear();
		}

		if (!s_mouseToUpdate.IsEmpty())
		{
			for (int mouseButton : s_mouseToUpdate)
			{
				s_mouseStates[mouseButton].previouslyDown = s_mouseStates[mouseButton].currentlyDown;
			}

			s_mouseToUpdate.Clear();
		}

		s_isKeyPressed = false;
		*/

		s_keyboard.OnUpdate();
		s_mouse.OnUpdate();
	}

	bool Input::OnKeyPressedEvent(KeyPressedEvent& keyPressed)
	{
		int keyCode = (int)keyPressed.GetKeyCode();
		if (!s_keyStates.ContainsKey(keyCode))
		{
			InputState state = { true, false };
			s_keyStates.Add((int)keyCode, state);
		}
		else
		{
			s_keyStates[keyCode].currentlyDown = true;
		}
		s_keysToUpdate.Add(keyCode);
		s_numKeyDown++;
		s_isKeyPressed = true;
		return false;
	}

	bool Input::OnKeyReleasedEvent(KeyReleasedEvent& keyReleased)
	{
		int keyCode = (int)keyReleased.GetKeyCode();
		if (!s_keyStates.ContainsKey(keyCode))
		{
			InputState state = { false, true };
			s_keyStates.Add(keyCode, state);
		}
		else
		{
			s_keyStates[keyCode].currentlyDown = false;
		}
		s_keysToUpdate.Add(keyCode);
		s_numKeyDown--;
		return false;
	}

	bool Input::OnMousePressedEvent(MouseButtonPressedEvent& mousePressed)
	{
		int buttonCode = (int)mousePressed.GetButtonKeycode();
		if (!s_mouseStates.ContainsKey(buttonCode))
		{
			InputState state = { true, false };
			s_mouseStates.Add(buttonCode, state);
		}
		else
		{
			s_mouseStates[buttonCode].currentlyDown = true;
		}
		s_mouseToUpdate.Add(buttonCode);
		return false;
	}

	bool Input::OnMouseReleasedEvent(MouseButtonReleasedEvent& mouseReleased)
	{
		int buttonCode = (int)mouseReleased.GetButtonKeycode();
		if (!s_mouseStates.ContainsKey(buttonCode))
		{
			InputState state = { false, true };
			s_mouseStates.Add(buttonCode, state);
		}
		else
		{
			s_mouseStates[buttonCode].currentlyDown = false;
		}
		s_mouseToUpdate.Add(buttonCode);
		return false;
	}
}

#endif // AE_PLATFORM_WINDOWS