#include "aepch.h"

#ifdef AE_PLATFORM_WINDOWS

#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Application.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	bool Input::IsKeyDown(int keycode)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetApp()->GetWindow()->GetNativeWindow();
		int status = glfwGetKey(window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(KeyCode key)
	{
		return IsKeyDown((int)key);
	}

	bool Input::IsKeyPressed(int keycode)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetApp()->GetWindow()->GetNativeWindow();
		int status = glfwGetKey(window, keycode);
		return status == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(KeyCode key)
	{
		return IsKeyPressed((int) key);
	}

	bool Input::IsMouseButtonPressed(int mouseButton)
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetApp()->GetWindow()->GetNativeWindow();
		int status = glfwGetMouseButton(window, mouseButton);
		return status == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonPressed(MouseButtonCode mouseButton)
	{
		return IsMouseButtonPressed((int) mouseButton);
	}

	Vector2 Input::GetMousePosition() 
	{
		GLFWwindow* window = (GLFWwindow*)Application::GetApp()->GetWindow()->GetNativeWindow();
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
}

#endif // AE_PLATFORM_WINDOWS