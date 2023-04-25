#pragma once
#include "AstralEngine/AEvents/AEvent.h"
#include <sstream>

namespace AstralEngine
{
	class AppEvent : public AEvent
	{
		DEF_AEVENT_CATEGORY(AEventCategory::AEventCategoryApplication);
	};

	class AppTickEvent : public AppEvent
	{
	public:
		AppTickEvent() { }
		~AppTickEvent() { }

		std::string ToString() const override
		{
			return "AppTick";
		}

		DEF_AEVENT_TYPE(AppTick);
	};

	class AppRenderEvent : public AppEvent
	{
	public:
		AppRenderEvent() { }
		~AppRenderEvent() { }

		std::string ToString() const override
		{
			return "AppRender";
		}

		DEF_AEVENT_TYPE(AppRender);
	};

	class AppUpdateEvent : public AppEvent
	{
	public:
		AppUpdateEvent() { }
		~AppUpdateEvent() { }

		std::string ToString() const override
		{
			return "AppUpdate";
		}

		DEF_AEVENT_TYPE(AppUpdate);
	};

	class WindowResizeEvent : public AEvent
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height) { }
		~WindowResizeEvent() { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResize New Size: " << m_width << "x" << m_height;
			return ss.str();
		}

		inline unsigned int GetWidth() { return m_width; }
		inline unsigned int GetHeight() { return m_height; }

		DEF_AEVENT_TYPE(WindowResize);
		DEF_AEVENT_CATEGORY(AEventCategory::AEventCategoryApplication);

	private:
		unsigned int m_width, m_height;
	};

	class WindowCloseEvent : public AppEvent
	{
	public:
		WindowCloseEvent() { }
		~WindowCloseEvent() { }

		std::string ToString() const override
		{
			return "WindowClose";
		}

		DEF_AEVENT_TYPE(WindowClose);
	};

	class WindowFocusEvent : public AppEvent
	{
	public:
		WindowFocusEvent() { }
		~WindowFocusEvent() { }

		std::string ToString() const override
		{
			return "WindowFocus";
		}

		DEF_AEVENT_TYPE(WindowFocus);
	};

	class WindowLostFocusEvent : public AppEvent
	{
	public:
		WindowLostFocusEvent() { }
		~WindowLostFocusEvent() { }

		std::string ToString() const override
		{
			return "WindowLostFocus";
		}

		DEF_AEVENT_TYPE(WindowLostFocus);
	};

	class WindowMovedEvent : public AEvent
	{
	public:
		WindowMovedEvent(int xPos, int yPos) : m_xPos(xPos), m_yPos(yPos) { }
		~WindowMovedEvent() { }

		inline int GetXPos() { return m_xPos; }
		inline int GetYPos() { return m_yPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMoved Position: (" << m_xPos << ", " << m_yPos << ")";
			return ss.str();
		}

		DEF_AEVENT_CATEGORY(AEventCategory::AEventCategoryApplication);
		DEF_AEVENT_TYPE(WindowMoved);

	private:
		int m_xPos, m_yPos;
	};
}