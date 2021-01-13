#include "aepch.h"
#include "WindowsWindow.h"
#include "AstralEngine/Core/Application.h"

#include <glad/glad.h>

namespace AstralEngine
{
	bool WindowsWindow::s_glfwInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		AE_CORE_ERROR("GLFW Error (%d): %s", error, description);
	}

	AWindow* AWindow::Create(const std::string& title, unsigned int width, unsigned int height)
	{
		return new WindowsWindow(title, width, height);
	}

	WindowsWindow::WindowsWindow(const std::string& title, unsigned int width, unsigned int height) : m_data(title, width, height)
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		AE_PROFILE_FUNCTION();
		delete m_context;
		ShutDown();
	}

	void WindowsWindow::SetEventCallback(AEventCallback callback)
	{
		m_data.callback = callback;
	}

	void WindowsWindow::OnUpdate()
	{
		AE_PROFILE_FUNCTION();
		glfwPollEvents();
		m_context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled) 
	{
		AE_PROFILE_FUNCTION();
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_VSync = enabled;
	}

	void WindowsWindow::Init()
	{
		AE_PROFILE_FUNCTION();
		if (!s_glfwInitialized)
		{
			AE_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			AE_CORE_ASSERT(success != GLFW_FALSE, "GLFW could not initialize properly.");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_glfwInitialized = true;
		}

		{
			AE_PROFILE_SCOPE("glfwCreateWindow");
			m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
		}

		m_context = new OpenGLGraphicsContext(m_window);

		m_context->Init();
		SetVSync(true);

		glfwSetWindowUserPointer(m_window, &m_data);

		
		//set callbacks from glfw
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* win, int width, int height)
			{
				WindowData* data = (WindowData*) glfwGetWindowUserPointer(win);
				data->width = width;
				data->width = height;

				WindowResizeEvent resizedEvent(width, height);
				data->callback(resizedEvent);
			});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* win)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);
				WindowCloseEvent close;
				data->callback(close);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* win, int key, int scanCode, int action, int mods)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);

				switch(action)
				{
					{
						case GLFW_PRESS:
							KeyPressedEvent keyPressed(key, 0);
							data->callback(keyPressed);
							break;
					}

					{
						case GLFW_REPEAT:
							KeyPressedEvent keyPressed(key, 1);
							data->callback(keyPressed);
							break;
					}

					{
						case GLFW_RELEASE:
							KeyReleasedEvent keyReleased(key);
							data->callback(keyReleased);
							break;
					}
				}
			});

		glfwSetCharCallback(m_window, [](GLFWwindow* win, unsigned int keycode)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);
				KeyTypedEvent keyTyped(keycode);
				data->callback(keyTyped);
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* win, int button, int action, int mods)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);

				switch (action)
				{
					{
						case GLFW_PRESS:
							MouseButtonPressedEvent buttonPressed(button);
							data->callback(buttonPressed);
							break;
					}

					{
						case GLFW_RELEASE:
							MouseButtonReleasedEvent buttonReleased(button);
							data->callback(buttonReleased);
							break;
					}
				}
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* win, double xOffset, double yOffset)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);
				MouseScrolledEvent mouseScroll((float) xOffset, (float) yOffset);
				data->callback(mouseScroll);
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* win, double xPos, double yPos) 
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(win);
				MouseMovedEvent mouseMove((float) xPos, (float) yPos);
				data->callback(mouseMove);
			});
	}

	void WindowsWindow::ShutDown()
	{
		AE_PROFILE_FUNCTION();
		glfwDestroyWindow(m_window);
	}
}