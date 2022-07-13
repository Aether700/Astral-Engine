#include "aepch.h"
#include "WindowsWindow.h"
#include "AstralEngine/Core/Application.h"
#include "Renderer/RenderAPI.h"

#include <glad/glad.h>

namespace AstralEngine
{
	// AWindow::Create function

	AWindow* AWindow::Create(const std::string& title, unsigned int width, unsigned int height)
	{
		return new WindowsWindow(title, width, height);
	}

	// WindowProceedure /////////////////////////////////////////////////////////

	LRESULT WindowProceedure(HWND window, UINT message,
		WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow* windowObj = (WindowsWindow*)GetWindowLongPtr(window, GWLP_USERDATA);

		switch (message)
		{
			case WM_CREATE:
				{
					// Setup user data when creating the window
					CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
					WindowsWindow* userData = reinterpret_cast<WindowsWindow*>(createStruct->lpCreateParams);
					
					SetLastError(0);
					if (SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)userData) == 0)
					{
						DWORD errorCode = GetLastError();
						if (errorCode != 0)
						{
							AE_CORE_ERROR("Could not set window user data. Error code: %L", errorCode);
							return false;
						}
					}
				}
				break;

			case WM_SIZE:
				{
					constexpr size_t wordSize = sizeof(LPARAM) / 2;

					if (windowObj->m_callback == nullptr)
					{
						break;
					}

					std::uint64_t lowerWord;
					std::uint64_t higherWord;
					memcpy(&lowerWord, &lParam, wordSize);
					memcpy(&higherWord, (&lParam) + wordSize, wordSize);
					WindowResizeEvent resizeEvent = WindowResizeEvent((unsigned int)lowerWord, 
						(unsigned int)higherWord);
					windowObj->m_callback(resizeEvent);
				}
				break;


		}

		return DefWindowProc(window, message, wParam, lParam);
	}

	// WindowsClass /////////////////////////////////////////////////////////////

	WindowsStr WindowsClass::GetWindowsClassName()
	{
		return GetInstance().m_className;
	}

	WindowsClass& WindowsClass::GetInstance()
	{
		static WindowsClass instance = Initialize();
		return instance;
	}

	WindowsClass WindowsClass::Initialize()
	{
		WindowsClass win;
		win.m_className = StrToWindowsStr("WindowsWindow");

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = WindowProceedure;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = win.m_className.c_str();
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);

		return win;
	}


	WindowsWindow::WindowsWindow(const std::string& title, unsigned int width, unsigned int height) 
		: WindowsWindow(title, 0, 0, width, height)
	{
	}

	WindowsWindow::WindowsWindow(const std::string& title, unsigned int x, unsigned int y, 
		unsigned int width, unsigned int height)
	{
		m_handle = CreateWindowEx(
			0, WindowsClass::GetWindowsClassName().c_str(),
			StrToWindowsStr(title).c_str(),
			WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX | WS_VISIBLE,
			x, y, width, height,
			nullptr, nullptr, GetModuleHandle(nullptr), this
		);

		m_context = GraphicsContext::Create(this);
		m_context->Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		int result = DestroyWindow(m_handle);
		if (result == 0)
		{
			AE_CORE_ERROR("WindowsWindow could not be properly destroyed. Error code %L", GetLastError());
		}
	}

	unsigned int WindowsWindow::GetWidth() const
	{
		RECT r = GetRect();
		return r.right - r.left;
	}

	unsigned int WindowsWindow::GetHeight() const
	{
		RECT r = GetRect();
		return r.bottom - r.top;
	}

	std::string WindowsWindow::GetTitle() const 
	{
		return WindowsStrToStr(GetTitleWindowsStr());
	}

	std::wstring WindowsWindow::GetTitleWStr() const
	{
		return WindowsStrToWStr(GetTitleWindowsStr());
	}

	void WindowsWindow::OnUpdate()
	{
		ProcessEvents();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		switch(RenderAPI::GetAPI())
		{
			case RenderAPI::API::OpenGL:
				SetVSyncOpenGL(enabled);
				break;

			case RenderAPI::API::None:
				AE_CORE_WARN("Setting VSync. No RenderAPI selected");
				return;

			default:
				AE_CORE_ERROR("Unknown RenderAPI detected");
				break;
		}
	}

	void WindowsWindow::SetEventCallback(AEventCallback callback)
	{
		m_callback = callback;
	}

	void WindowsWindow::SetVisible(bool visible)
	{
		int cmd = visible ? SW_SHOW : SW_HIDE;
		ShowWindow(m_handle, cmd);
	}
	
	bool WindowsWindow::IsVisible() const
	{
		return IsWindowVisible(m_handle) != 0;
	}

	WindowsStr WindowsWindow::GetTitleWindowsStr() const
	{
		WindowsChar buffer[128];
		int result = GetWindowText(m_handle, buffer, 128);

		if (result == 0)
		{
			AE_CORE_ERROR("Could not retrieve WindowsWindow's title. Error code: %L", GetLastError());
		}
		return WindowsStr(buffer, result + 1);
	}

	RECT WindowsWindow::GetRect() const
	{
		RECT r;
		int result = GetWindowRect(m_handle, &r);

		if (result == 0)
		{
			AE_CORE_ERROR("Could not retrieve WindowsWindow RECT. Error code %L", GetLastError());
		}
		return r;
	}

	void WindowsWindow::ProcessEvents()
	{
		MSG message;
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message); // call window proceedure function for the window who generated the message
		}

	}

	void WindowsWindow::SetVSyncOpenGL(bool enabled)
	{
		AE_CORE_WARN("\n\n\nSetVSyncOpenGL not properly implemented yet. See function in the code for details\n\n");
		if (!CheckIfOpenGLExtensionIsSupported("WGL_EXT_swap_control"))
		{
			AE_CORE_WARN("Trying to enable/disable VSync but VSync is not supported on this machine");
			return;
		}

		ADelegate<bool(int)> swapIntervalFunc
			= RetrieveOpenGLExtensionFunction<bool, int>((const char*)"wglSwapIntervalEXT");
		if (swapIntervalFunc != nullptr)
		{
			// need to retrieve the window associated with the current context, change the window of the 
			//current context to be this window, swapIntervals and finally change the window to be the old  
			//current context window 

			int swapInterval = enabled ? 1 : 0;
			if (!swapIntervalFunc(swapInterval))
			{
				AE_CORE_ERROR("Could not enable/disable VSync. ErrorCode %L", GetLastError());
				return;
			}
		}
	}

	/*
	bool WindowsWindow::s_glfwInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		AE_CORE_ERROR("GLFW Error (%d): %s", error, description);
	}

	AWindow* AWindow::Create(const std::string& title, unsigned int width, unsigned int height)
	{
		return new WindowsWindow(title, width, height);
	}

	WindowsWindow::WindowsWindow(const std::string& title, unsigned int width, unsigned int height) 
		: m_data(title, width, height)
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
				data->height = height;

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
	*/
}