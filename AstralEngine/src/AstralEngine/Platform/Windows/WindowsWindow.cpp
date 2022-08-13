#include "aepch.h"

#ifdef AE_PLATFORM_WINDOWS

	#include "WindowsWindow.h"
	#include "AstralEngine/Core/Application.h"
	#include "AstralEngine/Renderer/RenderAPI.h"
	
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
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t doubleWord = (std::uint32_t)lParam;
							std::uint16_t& lowerWord = GetLowOrderWord(doubleWord);
							std::uint16_t& higherWord = GetHighOrderWord(doubleWord);
							WindowResizeEvent resizeEvent = WindowResizeEvent((unsigned int)lowerWord,
								(unsigned int)higherWord);
							windowObj->m_callback(resizeEvent);
						}
						return 0;
					}
					break;

				case WM_CLOSE:
					{
						if (windowObj->m_callback != nullptr)
						{
							WindowCloseEvent close;
							windowObj->m_callback(close);
						}
						return 0;
					}
					break;
	
				case WM_SETFOCUS:
					{
						if (windowObj->m_callback != nullptr)
						{
							WindowFocusEvent focus;
							windowObj->m_callback(focus);
						}
						return 0;
					}
					break;
	
				case WM_KILLFOCUS:
					{
						if (windowObj->m_callback != nullptr)
						{
							WindowLostFocusEvent focus;
							windowObj->m_callback(focus);
						}
						return 0;
					}
					break;

				case WM_MOVE:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t doubleWord = (std::uint32_t)lParam;
							std::int16_t& x = reinterpret_cast<std::int16_t&>(GetLowOrderWord(doubleWord));
							std::int16_t& y = reinterpret_cast<std::int16_t&>(GetHighOrderWord(doubleWord));
							WindowMovedEvent move = WindowMovedEvent(x, y);
							windowObj->m_callback(move);
						}
						return 0;
					}
					break;

				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					{
						if (windowObj->m_callback != nullptr)
						{
							KeyCode key = WindowsKeyCodesToInternalKeyCode(wParam);
							if (key != KeyCode::Count)
							{
								constexpr std::uint32_t previousStateMask = 1 << 30;
								bool wasPressed = lParam & previousStateMask;
								
								if (wasPressed)
								{
									KeyRepeatedEvent repeated = KeyRepeatedEvent(key);
									windowObj->m_callback(repeated);
								}
								else
								{
									KeyPressedEvent pressed = KeyPressedEvent(key);
									windowObj->m_callback(pressed);
								}
							}
						}
						return 0;
					}
					break;

				case WM_KEYUP:
				case WM_SYSKEYUP:
					{
						if (windowObj->m_callback != nullptr)
						{
							KeyCode key = WindowsKeyCodesToInternalKeyCode(wParam);
							if (key != KeyCode::Count)
							{
								KeyReleasedEvent released = KeyReleasedEvent(key);
								windowObj->m_callback(released);
							}
						}
						return 0;
					}
					break;

				case WM_MOUSEMOVE:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t& doubleWord = (std::uint32_t&)lParam;
							std::int16_t& x = reinterpret_cast<std::int16_t&>(GetLowOrderWord(doubleWord));
							std::int16_t& y = reinterpret_cast<std::int16_t&>(GetHighOrderWord(doubleWord));
							MouseMovedEvent move = MouseMovedEvent(x, y);
							windowObj->m_callback(move);
						}
						return 0;
					}
					break;

				case WM_MOUSEWHEEL:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t& doubleWord = (std::uint32_t&)wParam;
							std::int16_t& deltaY = reinterpret_cast<std::int16_t&>(GetHighOrderWord(doubleWord));
							MouseScrolledEvent scrolled = MouseScrolledEvent(0, deltaY / WHEEL_DELTA);
							windowObj->m_callback(scrolled);
						}
						return 0;
					}
					break;

				case WM_MOUSEHWHEEL:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t& doubleWord = (std::uint32_t&)wParam;
							std::int16_t& deltaX = reinterpret_cast<std::int16_t&>(GetHighOrderWord(doubleWord));
							MouseScrolledEvent scrolled = MouseScrolledEvent(deltaX / WHEEL_DELTA, 0);
							windowObj->m_callback(scrolled);
						}
						return 0;
					}
					break;

				case WM_LBUTTONDOWN:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonPressedEvent pressed = MouseButtonPressedEvent(MouseButtonCode::Left);
							windowObj->m_callback(pressed);
						}
						return 0;
					}
					break;

				case WM_LBUTTONUP:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonReleasedEvent released = MouseButtonReleasedEvent(MouseButtonCode::Left);
							windowObj->m_callback(released);
						}
						return 0;
					}
					break;

				case WM_RBUTTONDOWN:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonPressedEvent pressed = MouseButtonPressedEvent(MouseButtonCode::Right);
							windowObj->m_callback(pressed);
						}
						return 0;
					}
					break;

				case WM_RBUTTONUP:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonReleasedEvent released = MouseButtonReleasedEvent(MouseButtonCode::Right);
							windowObj->m_callback(released);
						}
						return 0;
					}
					break;

				case WM_MBUTTONDOWN:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonPressedEvent pressed = MouseButtonPressedEvent(MouseButtonCode::Middle);
							windowObj->m_callback(pressed);
						}
						return 0;
					}
					break;

				case WM_MBUTTONUP:
					{
						if (windowObj->m_callback != nullptr)
						{
							MouseButtonReleasedEvent released = MouseButtonReleasedEvent(MouseButtonCode::Middle);
							windowObj->m_callback(released);
						}
						return 0;
					}
					break;

				case WM_XBUTTONDOWN:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t& doubleWord = (std::uint32_t&)wParam;
							MouseButtonCode button 
								= WindowsMouseCodesToInternalMouseCode(GetHighOrderWord(doubleWord));

							if (button != MouseButtonCode::Count)
							{
								MouseButtonPressedEvent pressed = MouseButtonPressedEvent(button);
								windowObj->m_callback(pressed);
							}
						}
						return 0;
					}
					break;

				case WM_XBUTTONUP:
					{
						if (windowObj->m_callback != nullptr)
						{
							std::uint32_t& doubleWord = (std::uint32_t&)wParam;
							MouseButtonCode button
								= WindowsMouseCodesToInternalMouseCode(GetHighOrderWord(doubleWord));

							if (button != MouseButtonCode::Count)
							{
								MouseButtonReleasedEvent released = MouseButtonReleasedEvent(button);
								windowObj->m_callback(released);
							}
						}
						return 0;
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
			unsigned int width, unsigned int height) : m_isFullscreen(false), m_fullscreenInfo(nullptr)
		{
			m_handle = CreateWindowEx(
				0, WindowsClass::GetWindowsClassName().c_str(),
				StrToWindowsStr(title).c_str(),
				WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX | WS_VISIBLE,
				x, y, width, height,
				nullptr, nullptr, GetModuleHandle(nullptr), this
			);
	
			if (m_handle == nullptr)
			{
				AE_CORE_ERROR("Could not initialize WindowsWindow. Error Code: %L", GetLastError());
			}
			else
			{
				m_context = GraphicsContext::Create(this);
				m_context->Init();
			}
		}
	
		WindowsWindow::~WindowsWindow()
		{
			delete m_fullscreenInfo;
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

		void WindowsWindow::SetTitle(const std::string& title)
		{
			if (SetWindowText(m_handle, StrToWindowsStr(title).c_str()) == 0)
			{
				AE_CORE_ERROR("Could not change the window title. Error code %L", GetLastError());
			}
		}

		void WindowsWindow::SetTitle(const std::wstring& title)
		{
			if (SetWindowText(m_handle, WStrToWindowsStr(title).c_str()) == 0)
			{
				AE_CORE_ERROR("Could not change the window title. Error code %L", GetLastError());
			}
		}
	
		void WindowsWindow::OnUpdate()
		{
			ProcessEvents();
			m_context->SwapBuffers();
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
	
		void WindowsWindow::SetMaximize(bool isMaximized)
		{
			if (isMaximized && !IsMaximized())
			{
				ShowWindow(m_handle, SW_MAXIMIZE);
			}
			else if (!isMaximized && IsMaximized())
			{ 
				ShowWindow(m_handle, SW_RESTORE);
			}
		}

		bool WindowsWindow::IsMaximized() const 
		{
			WINDOWPLACEMENT placement = RetrieveWindowPlacement();
			return placement.showCmd == SW_MAXIMIZE;
		}
		
		void WindowsWindow::SetMinimize(bool isMinimized)
		{
			if (isMinimized && !IsMinimized())
			{
				ShowWindow(m_handle, SW_MINIMIZE);
			}
			else if (!isMinimized && IsMinimized())
			{
				ShowWindow(m_handle, SW_RESTORE);
			}
		}

		bool WindowsWindow::IsMinimized() const 
		{
			WINDOWPLACEMENT placement = RetrieveWindowPlacement();
			return placement.showCmd == SW_MINIMIZE;
		}

		void WindowsWindow::SetFullscreen(bool isFullscreen)
		{
			if (isFullscreen && !IsFullscreen())
			{
				m_fullscreenInfo = new WindowFullscreenInfo();
				m_fullscreenInfo->style = GetWindowLong(m_handle, GWL_STYLE);

				if (GetWindowPlacement(m_handle, &m_fullscreenInfo->placement) == 0)
				{
					AE_CORE_ERROR("Could not retrieve window information. Error Code: %L", GetLastError());
					return;
				}

				HMONITOR monitorHandle = MonitorFromWindow(m_handle, MONITOR_DEFAULTTOPRIMARY);
				MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
				if (GetMonitorInfo(monitorHandle, &monitorInfo) == 0)
				{
					AE_CORE_ERROR("Could not retrieve monitor information.");
					return;
				}
				
				SetWindowLong(m_handle, GWL_STYLE, m_fullscreenInfo->style & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(m_handle, nullptr, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

				m_isFullscreen = true;
			}
			else if (!isFullscreen && IsFullscreen())
			{
				AE_CORE_ASSERT(m_fullscreenInfo != nullptr, "");
				SetWindowLong(m_handle, GWL_STYLE, m_fullscreenInfo->style);
				SetWindowPlacement(m_handle, &m_fullscreenInfo->placement);
				SetWindowPos(m_handle, nullptr, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				m_isFullscreen = false;
				delete m_fullscreenInfo;
				m_fullscreenInfo = nullptr;
			}
		}

		bool WindowsWindow::IsFullscreen() const { return m_isFullscreen; }

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
	
		WINDOWPLACEMENT WindowsWindow::RetrieveWindowPlacement() const
		{
			WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
			if (GetWindowPlacement(m_handle, &placement) == 0)
			{
				AE_CORE_ERROR("Could not retrieve window information. Error Code: %L", GetLastError());
			}
			return placement;
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
#endif