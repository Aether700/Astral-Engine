#pragma once

#ifdef AE_PLATFORM_WINDOWS
	#include "AstralEngine/Core/AWindow.h"
	#include "AstralEngine/Renderer/GraphicsContext.h"
	
	#include "WindowsUtil.h"

	namespace AstralEngine
	{
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
			virtual std::wstring GetTitleWStr() const override;
			virtual void SetTitle(const std::string& title) override;
			virtual void SetTitle(const std::wstring& title) override;
	
			virtual void OnUpdate() override;
	
			virtual void SetVSync(bool enabled) override;
			virtual void SetEventCallback(AEventCallback callback) override;
	
			virtual void SetVisible(bool isVisible) override;
			virtual bool IsVisible() const override;
	
			virtual void SetMaximize(bool isMaximized) override;
			virtual bool IsMaximized() const override;

			virtual void SetMinimize(bool isMinimized) override;
			virtual bool IsMinimized() const override;

			virtual void SetFullscreen(bool isFullscreen) override;
			virtual bool IsFullscreen() const override;

		private:

			// information used to restore a full screen window to it's previous style/format
			struct WindowFullscreenInfo
			{
				DWORD style;
				WINDOWPLACEMENT placement;
			};

			friend LRESULT WindowProceedure(HWND window, UINT message,
				WPARAM wParam, LPARAM lParam);
	
			WindowsStr GetTitleWindowsStr() const;
			RECT GetRect() const;
	
			WINDOWPLACEMENT RetrieveWindowPlacement() const;

			static void ProcessEvents();
	
			void SetVSyncOpenGL(bool enabled);
	
			HWND m_handle;
			AEventCallback m_callback;
			AReference<GraphicsContext> m_context;
			WindowFullscreenInfo* m_fullscreenInfo;
			bool m_isFullscreen;
		};
	}
#endif