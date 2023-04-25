#include "aepch.h"
#include "OpenGLGraphicsContext.h"
#include "AstralEngine/Core/AWindow.h"

#include <glad/glad.h>

#ifdef AE_PLATFORM_WINDOWS
	#include "AstralEngine/Platform/Windows/WindowsUtil.h"
	#include "AstralEngine/Platform/Windows/WindowsWindow.h"
	#include "../vendor/OpenGL/wglext.h"
#endif

namespace AstralEngine
{
	// helpers ////////////////////////////////////////////////////////////////////////////////////////////////////
	
#ifdef AE_PLATFORM_WINDOWS

	HGLRC(*wglCreateContextAttribsARB) (HDC, HGLRC, const int*) = nullptr;
	BOOL(*wglChoosePixelFormatARB) (HDC, const int*, const FLOAT*, UINT, int*, UINT*) = nullptr;

	void InitializeARBFunctions()
	{
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		WindowsStr tempClassName = StrToWindowsStr("Temporary Window");

		WNDCLASSEX wc = { 0 };
		
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = tempClassName.c_str();
		wc.hIconSm = nullptr;
		
		RegisterClassEx(&wc);

		HWND tempWindow = CreateWindowEx(
			0, tempClassName.c_str(),
			StrToWindowsStr("Temporary Window").c_str(), 0,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr,
			nullptr, hInstance, nullptr
		);

		if (tempWindow == nullptr)
		{
			AE_CORE_ERROR("Could not Create Windows OpenGL Context. Error Code %L", GetLastError());
			return;
		}

		HDC tempDeviceContext = GetDC(tempWindow);
		if (tempDeviceContext == nullptr)
		{
			AE_CORE_ERROR("Could not Create Windows OpenGL Context. Error Code %L", GetLastError());
			return;
		}

		// specifies the requested pixel format. This is the "ideal" pixel format and might not 
		// be identical to the provided format
		PIXELFORMATDESCRIPTOR formatDescriptor =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_SWAP_LAYER_BUFFERS,
			PFD_TYPE_RGBA, 24,
			0, 0, 0, 0, 0, 0, // ignore color bits
			0, 0, // no alpha bit or shift
			0, 0, 0, 0, 0, // no accumulation buffer or bits
			32, 32, // 32 bit depth buffer and 32 bit stencil buffer
			0, // no auxilary buffer
			PFD_MAIN_PLANE,
			0, // reserved
			0, 0, 0 // no masks
		};

		int tempPixelFormat = ChoosePixelFormat(tempDeviceContext, &formatDescriptor);
		if (tempPixelFormat == 0)
		{
			AE_CORE_ERROR("Could not find proper pixel format. Error Code: %L", GetLastError());
			return;
		}

		if (!SetPixelFormat(tempDeviceContext, tempPixelFormat, &formatDescriptor))
		{
			AE_CORE_ERROR("Could not set pixel format. Error Code: %L", GetLastError());
			return;
		}

		HGLRC tempOpenglContext = wglCreateContext(tempDeviceContext);
		if (tempOpenglContext == nullptr)
		{
			AE_CORE_ERROR("Could not create graphics context. Error Code: %L", GetLastError());
			return;
		}

		if (!wglMakeCurrent(tempDeviceContext, tempOpenglContext))
		{
			AE_CORE_ERROR("Could not make context current. Error Code: %L", GetLastError());
			return;
		}

		wglCreateContextAttribsARB
			= (HGLRC(*) (HDC, HGLRC, const int*))wglGetProcAddress("wglCreateContextAttribsARB");

		if (wglCreateContextAttribsARB == nullptr)
		{
			AE_CORE_ERROR("Could not load Windows ARB functions. Error Code: %L", GetLastError());
			return;
		}

		wglChoosePixelFormatARB
			= (BOOL(*)(HDC, const int*, const FLOAT*, 
				UINT, int*, UINT*))wglGetProcAddress("wglChoosePixelFormatARB");

		if (wglChoosePixelFormatARB == nullptr)
		{
			AE_CORE_ERROR("Could not load Windows ARB functions. Error Code: %L", GetLastError());
			return;
		}

		wglMakeCurrent(tempDeviceContext, nullptr);
		wglDeleteContext(tempOpenglContext);
		ReleaseDC(tempWindow, tempDeviceContext);
		DestroyWindow(tempWindow);
	}
#endif

	static void OpenGLErrorCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
		const char* message, const void* userPtr)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			AE_CORE_ERROR("High Severity: %s", message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			AE_CORE_ERROR("High Medium: %s", message);
			break;

		case GL_DEBUG_SEVERITY_LOW:
			AE_CORE_WARN("High LOW: %s", message);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			AE_CORE_INFO("NOTIFICATION: %s", message);
			break;
		}
	}

	void* ProceedureLoader(const char* functionName)
	{
		#ifdef AE_PLATFORM_WINDOWS
			return RetrieveOpenGLFunction(functionName);
		#else
			#error no platform macro defined
		#endif
	}


	// OpenGLGraphicsContext ///////////////////////////////////////////////////////////////////////////////////

	OpenGLGraphicsContext::OpenGLGraphicsContext(AWindow* window) : m_window(window) { }
	
	OpenGLGraphicsContext::~OpenGLGraphicsContext() 
	{
		#ifdef AE_PLATFORM_WINDOWS
			HGLRC currContext = wglGetCurrentContext();
			if (currContext == m_nativeContext)
			{
				wglCreateContext(nullptr);
			}
		
			wglDeleteContext(m_nativeContext);
		
		#else
			#error no platform macro defined 
		#endif
	}

	void OpenGLGraphicsContext::Init() 
	{
		AE_PROFILE_FUNCTION();
		
		m_nativeContext = CreateNativeContext(m_window);
		MakeContextCurrent();

		int status = gladLoadGLLoader((GLADloadproc)ProceedureLoader);
		AE_CORE_ASSERT(status, "Failed to initialize Glad");

		//Enable Error callback to display messages
		glDebugMessageCallback(OpenGLErrorCallback, nullptr); 
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		
		// set opengl to be left handed instead of the default right handed
		glDepthRange(1.0, 0.0);
		glFrontFace(GL_CW);
		glDepthFunc(GL_GEQUAL);
		glClearDepth(0.0);

		AE_CORE_INFO("OpenGL Info:");
		AE_CORE_INFO("   Vendor: %s", glGetString(GL_VENDOR));
		AE_CORE_INFO("   Renderer: %s", glGetString(GL_RENDERER));
		AE_CORE_INFO("   Version: %s\n", glGetString(GL_VERSION));
	}

	void OpenGLGraphicsContext::SwapBuffers() 
	{
		//glfwSwapBuffers(m_window);
		#ifdef AE_PLATFORM_WINDOWS
			HWND windowHandle = (HWND)m_window->GetNativeWindow();
			HDC deviceContextHandle = GetDC(windowHandle);
			if (! ::SwapBuffers(deviceContextHandle))
			{
				AE_CORE_ERROR("Could not swap buffers. Error Code: %L", GetLastError());
			}
		#else
			#error no platform macro defined 
		#endif
	}

	NativeOpenGLContext OpenGLGraphicsContext::CreateNativeContext(AWindow* window) const
	{
		#ifdef AE_PLATFORM_WINDOWS
			HWND windowHandle = (HWND)window->GetNativeWindow();
			HDC deviceContextHandle = GetDC(windowHandle);

			if (wglCreateContextAttribsARB == nullptr)
			{
				InitializeARBFunctions();
			}

			int pixelFormatAttrib[] = {
				WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
				WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
				WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB,         32,
				WGL_DEPTH_BITS_ARB,         24,
				WGL_STENCIL_BITS_ARB,       8,
				0 // marks the end of the attributes
			};

			int pixelFormat;
			UINT numFormats;
			if (!wglChoosePixelFormatARB(deviceContextHandle, pixelFormatAttrib, nullptr, 1, 
				&pixelFormat, &numFormats))
			{
				AE_CORE_ERROR("Could not find proper pixel format");
				return nullptr;
			}

			PIXELFORMATDESCRIPTOR formatDescriptor;
			if (DescribePixelFormat(deviceContextHandle, pixelFormat, 
				sizeof(formatDescriptor), &formatDescriptor) == 0)
			{
				AE_CORE_ERROR("Could not find proper descriptor for the pixel format. Error Code: %L", GetLastError());
				return nullptr;
			}

			if (!SetPixelFormat(deviceContextHandle, pixelFormat, &formatDescriptor))
			{
				AE_CORE_ERROR("Could not set pixel format. Error Code: %L", GetLastError());
				return nullptr;
			}
		

			int openGLAttrib[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0 // marks the end of the attributes
			};
		

			HGLRC openglContext = wglCreateContextAttribsARB(deviceContextHandle, nullptr, openGLAttrib);
		
			if (openglContext == nullptr)
			{
				AE_CORE_ERROR("Could not create OpenGL Context. Error Code: %L", GetLastError());
				return nullptr;
			}
		
			return openglContext;

		#else
			#error no platform macro defined 
		#endif
	}

	void OpenGLGraphicsContext::MakeContextCurrent() const
	{
		#ifdef AE_PLATFORM_WINDOWS

			HWND windowHandle = (HWND)m_window->GetNativeWindow();
			HDC deviceContextHandle = GetDC((HWND)m_window->GetNativeWindow());
			if (!wglMakeCurrent(deviceContextHandle, m_nativeContext))
			{
				AE_CORE_ERROR("Could not make context current. Error Code: %L", GetLastError());
			}
			
			if (ReleaseDC(windowHandle, deviceContextHandle) == 0)
			{
				AE_CORE_ERROR("Could not properly release Device Context. Error Code: %L", GetLastError());
			}

		#else
			#error no platform macro defined 
		#endif
	}

}