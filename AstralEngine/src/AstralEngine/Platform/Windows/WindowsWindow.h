#pragma once
#include "AstralEngine/Core/AWindow.h"
#include "AstralEngine/Platform/OpenGL/OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	class WindowsWindow : public AWindow
	{
	public:
		WindowsWindow(const std::string& title, unsigned int width, unsigned int height);

		~WindowsWindow();

		inline virtual void* GetNativeWindow() override { return m_window; }
		inline virtual unsigned int GetWidth() const override { return m_data.width; }
		inline virtual unsigned int GetHeight() const override { return m_data.height; }
		inline virtual const std::string& GetTitle() const override { return m_data.title; }


		virtual void OnUpdate() override;

		virtual void SetVSync(bool enabled) override;
		virtual void SetEventCallback(AEventCallback callback) override;

	private:

		struct WindowData
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			AEventCallback callback;

			WindowData(const std::string& t, unsigned int w, unsigned h) : title(t), width(w), height(h) { }
		};

		void Init();
		void ShutDown();

		bool m_VSync;

		GLFWwindow* m_window;
		OpenGLGraphicsContext* m_context;
		WindowData m_data;

		static bool s_glfwInitialized;
	};
}