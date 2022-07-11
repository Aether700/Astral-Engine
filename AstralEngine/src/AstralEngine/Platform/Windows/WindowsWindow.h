#pragma once
#include "AstralEngine/Core/AWindow.h"
#include "AstralEngine/Platform/OpenGL/OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
#ifdef UNICODE
	typedef std::wstring WindowsStr;
	typedef wchar_t WindowsChar;
#else
	typedef std::string WindowsStr;
	typedef char WindowsChar;
#endif

	// Object responsible for registering the WindowsWindow class with windows
	class WindowsClass
	{
	public:
		static WindowsStr GetWindowsClassName();

	private:
		static WindowsClass& GetInstance();
		static WindowsClass Initialize();

		WindowsStr m_className;
	};


	class WindowsWindow : public AWindow
	{
	public:
		WindowsWindow(const std::string& title, unsigned int width, unsigned int height);
		WindowsWindow(const std::string& title, unsigned int x, unsigned int y, 
			unsigned int width, unsigned int height);

		~WindowsWindow();

		virtual void* GetNativeWindow() override { return m_handle; }
		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;
		virtual std::string GetTitle() const override;
		std::wstring GetTitleWStr() const;

		virtual void OnUpdate() override;

		virtual void SetVSync(bool enabled) override;
		virtual void SetEventCallback(AEventCallback callback) override;

		virtual void SetVisible(bool isVisible) override;
		virtual bool IsVisible() const override;

	private:
		WindowsStr GetTitleWindowsStr() const;
		RECT GetRect() const;

		HWND m_handle;
	};
	/*
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
	*/
}