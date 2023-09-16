#pragma once
#include <functional>
#include "AstralEngine/AEvents/AEvent.h"
#include "AstralEngine/Data Struct/ADelegate.h"

namespace AstralEngine
{
	class AWindow
	{
	public:
		using AEventCallback = ADelegate<void(AEvent&)>;
		virtual ~AWindow() { }
		virtual void* GetNativeWindow() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		float GetAspectRatio() const;
		virtual std::string GetTitle() const = 0;
		virtual std::wstring GetTitleWStr() const = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetTitle(const std::wstring& title) = 0;

		virtual void OnUpdate() = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual void SetEventCallback(AEventCallback callback) = 0;

		virtual void SetVisible(bool isVisible) = 0;
		virtual bool IsVisible() const = 0;

		virtual void SetMaximize(bool isMaximized) = 0;
		virtual bool IsMaximized() const = 0;

		virtual void SetMinimize(bool isMinimized) = 0;
		virtual bool IsMinimized() const = 0;

		virtual void SetFullscreen(bool isFullscreen) = 0;
		virtual bool IsFullscreen() const = 0;

		static AWindow* Create(const std::string& title, unsigned int width, unsigned int height);
	};
}