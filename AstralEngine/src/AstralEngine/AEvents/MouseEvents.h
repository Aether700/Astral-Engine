#pragma once
#include "AEvent.h"
#include "AstralEngine/Core/MouseButtonCodes.h"
#include <sstream>

namespace AstralEngine
{
	class MouseMovedEvent : public AEvent
	{
	public:
		MouseMovedEvent(int xPos, int yPos) : m_xPos(xPos), m_yPos(yPos) {	}
		~MouseMovedEvent() { }

		inline int GetXPos() const { return m_xPos; }
		inline int GetYPos() const { return m_yPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss  << "MouseMoved Position: (" << m_xPos << ", " << m_yPos << ")";
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseMoved);
		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput | AEventCategory::AEventCategoryMouse));
	private:
		int m_xPos, m_yPos;
	};

	class MouseScrolledEvent : public AEvent
	{
	public:
		MouseScrolledEvent(int offsetX, int offsetY) : m_offsetX(offsetX), m_offsetY(offsetY) { }
		~MouseScrolledEvent() { }

		inline int GetOffsetX() const { return m_offsetX; }
		inline int GetOffsetY() const { return m_offsetY; }

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
		MouseButtonEvent(MouseButtonCode keycode) : m_keycode(keycode) { }
		virtual ~MouseButtonEvent() { }

		inline MouseButtonCode GetButtonKeycode() const { return m_keycode; }

		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput |
			AEventCategory::AEventCategoryMouseButton | AEventCategory::AEventCategoryMouse));

	private:
		MouseButtonCode m_keycode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButtonCode keycode) : MouseButtonEvent(keycode) { }
		~MouseButtonPressedEvent() { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressed Keycode: " << (int)GetButtonKeycode();
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseButtonPressed);
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButtonCode keycode) : MouseButtonEvent(keycode) { }
		~MouseButtonReleasedEvent() { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedG Keycode: " << (int)GetButtonKeycode();
			return ss.str();
		}

		DEF_AEVENT_TYPE(MouseButtonReleased);
	};

}