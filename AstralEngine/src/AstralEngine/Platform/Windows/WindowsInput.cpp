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

	static AUnorderedMap<int, InputState> s_keyStates;
	static ADynArr<int> s_keysToUpdate;

	static AUnorderedMap<int, InputState> s_mouseStates;
	static ADynArr<int> s_mouseToUpdate(3);

	bool Input::GetKeyDown(int keycode)
	{
		if (!s_keyStates.ContainsKey(keycode))
		{
			return false;
		}
		return s_keyStates[keycode].currentlyDown && !s_keyStates[keycode].previouslyDown;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		return GetKeyDown((int)key);
	}

	bool Input::GetKey(int keycode)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetWindow()->GetNativeWindow();
		int status = glfwGetKey(window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}

	bool Input::GetKey(KeyCode key)
	{
		return GetKey((int) key);
	}

	bool Input::GetMouseButton(int mouseButton)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetWindow()->GetNativeWindow();
		int status = glfwGetMouseButton(window, mouseButton);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}
	
	bool Input::GetMouseButton(MouseButtonCode mouseButton)
	{
		return GetMouseButton((int) mouseButton);
	}

	bool Input::GetMouseButtonDown(int mouseButton)
	{
		if (!s_mouseStates.ContainsKey(mouseButton))
		{
			return false;
		}
		return s_mouseStates[mouseButton].currentlyDown 
			&& !s_mouseStates[mouseButton].previouslyDown;
	}

	bool Input::GetMouseButtonDown(MouseButtonCode mouseButton)
	{
		return GetMouseButtonDown((int)mouseButton);
	}

	Vector2 Input::GetMousePosition() 
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetWindow()->GetNativeWindow();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float) xPos, (float) yPos };
	}
	
	float Input::GetMouseXPos() 
	{
		Vector2 position = GetMousePosition();
		return position.x;
	}

	float Input::GetMouseYPos() 
	{
		Vector2 position = GetMousePosition();
		return position.y;
	}

	void Input::OnUpdate()
	{
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
	}

	bool Input::OnKeyPressedEvent(KeyPressedEvent& keyPressed)
	{
		int keyCode = keyPressed.GetKeyCode();
		if (!s_keyStates.ContainsKey(keyCode))
		{
			InputState state = { true, false };
			s_keyStates.Add(keyCode, state);
		}
		else
		{
			s_keyStates[keyCode].currentlyDown = true;
		}
		s_keysToUpdate.Add(keyCode);
		return false;
	}

	bool Input::OnKeyReleasedEvent(KeyReleasedEvent& keyReleased)
	{
		int keyCode = keyReleased.GetKeyCode();
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
		return false;
	}

	bool Input::OnMousePressedEvent(MouseButtonPressedEvent& mousePressed)
	{
		int buttonCode = mousePressed.GetButtonKeycode();
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
		int buttonCode = mouseReleased.GetButtonKeycode();
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