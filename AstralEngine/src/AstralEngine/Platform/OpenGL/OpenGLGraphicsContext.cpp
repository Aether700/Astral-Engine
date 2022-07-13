#include "aepch.h"
#include "OpenGLGraphicsContext.h"
#include "Core/AWindow.h"
#include "Platform/Windows/WindowsUtil.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace AstralEngine
{
	// helpers ////////////////////////////////////////////////////////////////////////////////////////////////////
	
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

	NativeOpenGLContext MakeWindowCurrentContext(AWindow* window)
	{
		#ifdef AE_PLATFORM_WINDOWS
			HWND windowHandle = (HWND)window->GetNativeWindow();
			HDC deviceContextHandle = GetDC(windowHandle);

			// specifies the requested pixel format. This is the "ideal" pixel format and might not 
			// be identical to the provided format
			PIXELFORMATDESCRIPTOR formatDescriptor =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_SWAP_LAYER_BUFFERS,
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
			
			// retrieve pixel format which best matches the desired pixel format
			int pixelFormat = ChoosePixelFormat(deviceContextHandle, &formatDescriptor);
			if (pixelFormat == 0)
			{
				AE_CORE_ERROR("Could not find proper pixel format. Error Code: %L", GetLastError());
				return nullptr;
			}


			if (!SetPixelFormat(deviceContextHandle, pixelFormat, &formatDescriptor))
			{
				AE_CORE_ERROR("Could not set pixel format. Error Code: %L", GetLastError());
				return nullptr;
			}

			HGLRC openglContext = wglCreateContext(deviceContextHandle);

			if (openglContext == nullptr)
			{
				AE_CORE_ERROR("Could not create OpenGL Context. Error Code: %L", GetLastError());
				return nullptr;
			}
			
			if (!wglMakeCurrent(deviceContextHandle, openglContext))
			{
				AE_CORE_ERROR("Could not make context current. Error Code: %L", GetLastError());
				return nullptr;
			}

			return openglContext;

		#else
			#error no platform macro defined 
		#endif
	}

	void DestroyGraphicsContext(NativeOpenGLContext context)
	{
		#ifdef AE_PLATFORM_WINDOWS
			HGLRC currContext = wglGetCurrentContext();
			if (currContext == context)
			{
				wglCreateContext(nullptr);
			}

			wglDeleteContext(context);

		#else
			#error no platform macro defined 
		#endif
	}

	void SwapWindowBuffers(AWindow* window)
	{
		#ifdef AE_PLATFORM_WINDOWS
			HWND windowHandle = (HWND)window->GetNativeWindow();
			HDC deviceContextHandle = GetDC(windowHandle);		
			if (!SwapBuffers(deviceContextHandle))
			{
				AE_CORE_ERROR("Could not swap buffers. Error Code: %L", GetLastError());
			}
		#else
			#error no platform macro defined 
		#endif
	}


	// OpenGLGraphicsContext ///////////////////////////////////////////////////////////////////////////////////

	OpenGLGraphicsContext::OpenGLGraphicsContext(AWindow* window) : m_window(window) { }
	
	OpenGLGraphicsContext::~OpenGLGraphicsContext() 
	{
		DestroyGraphicsContext(m_nativeContext);
	}


	void OpenGLGraphicsContext::Init() 
	{
		AE_PROFILE_FUNCTION();
		//glfwMakeContextCurrent(m_window);
		
		m_nativeContext = MakeWindowCurrentContext(m_window);


		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
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
		SwapWindowBuffers(m_window);
	}
}