#include "aepch.h"
#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace AstralEngine
{
	static void OpenGLErrorCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
		const char* message, const void* userPtr)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			AE_CORE_ERROR("High Severity: %s", message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			AE_CORE_ERROR("Medium Severity: %s", message);
			break;

		case GL_DEBUG_SEVERITY_LOW:
			AE_CORE_WARN("Low Severity: %s", message);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			AE_CORE_INFO("Notification: %s", message);
			break;
		}
	}


	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* window) : m_window(window){ }
	OpenGLGraphicsContext::~OpenGLGraphicsContext() { }


	void OpenGLGraphicsContext::Init() 
	{
		AE_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AE_CORE_ASSERT(status, "Failed to initialize Glad");

		//Enable Error callback to display messages
		glDebugMessageCallback(OpenGLErrorCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		
		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		glfwSwapBuffers(m_window);
	}
}