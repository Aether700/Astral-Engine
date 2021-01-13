#pragma once
#include "AstralEngine/Renderer/GraphicsContext.h"

extern struct GLFWwindow;

namespace AstralEngine
{
	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(GLFWwindow* window);
		~OpenGLGraphicsContext();

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_window;
	};
}