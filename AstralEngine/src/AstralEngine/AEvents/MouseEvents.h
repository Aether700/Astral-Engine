#pragma once
#include "AEvent.h"
#include <sstream>

namespace AstralEngine
{
	class MouseMovedEvent : public AEvent
	{
	public:
		MouseMovedEvent(float xPos, float yPos) : m_xPos(xPos), m_yPos(yPos) {	}
		~MouseMovedEvent() { }

		inline float GetXPos() const { return m_xPos; }
		inline float GetYPos() const { return m_yPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss  << "MouseMoved Position: (" << m_xPos << ", " << m_yPos << ")";
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseMoved);
		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput | AEventCategory::AEventCategoryMouse));
	private:
		float m_xPos, m_yPos;
	};

	class MouseScrolledEvent : public AEvent
	{
	public:
		MouseScrolledEvent(float offsetX, float offsetY) : m_offsetX(offsetX), m_offsetY(offsetY) { }
		~MouseScrolledEvent() { }

		inline float GetOffsetX() const { return m_offsetX; }
		inline float GetOffsetY() const { return m_offsetY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolled Offset: (" << m_offsetX << ", " << m_offsetY << ")";
			return ss.str();
		}

		DEF_AEVENT_CATEGORY((AEventCategory)(AEventCategory::AEventCategoryInput | AEventCategory::AEventCategoryMouse));
		DEF_AEVENT_TYPE(MouseScrolled);
	private:
		float m_offsetX;
		float m_offsetY;
	};

	class MouseButtonEvent : public AEvent
	{
	public:
		MouseButtonEvent(int keycode) : m_keycode(keycode) { }
		virtual ~MouseButtonEvent() { }

		inline int GetButtonKeycode() const { return m_keycode; }

		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput |
			AEventCategory::AEventCategoryMouseButton | AEventCategory::AEventCategoryMouse));

	private:
		int m_keycode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int keycode) : MouseButtonEvent(keycode) { }
		~MouseButtonPressedEvent() { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressed Keycode: " << GetButtonKeycode();
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseButtonPressed);
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int keycode) : MouseButtonEvent(keycode) { }
		~MouseButtonReleasedEvent() { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedG Keycode: " << GetButtonKeycode();
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseButtonReleased);
	};

}