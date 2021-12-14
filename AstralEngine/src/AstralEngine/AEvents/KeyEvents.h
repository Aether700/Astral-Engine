#pragma once
#include "AEvent.h"
#include <sstream>

namespace AstralEngine
{
	class KeyEvent : public AEvent
	{
	public:
		inline int GetKeyCode() const { return m_keycode; }

	protected:
		KeyEvent(int keycode) : m_keycode(keycode) { }

		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput | AEventCategory::AEventCategoryKeyboard));

		int m_keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_repeatCount(repeatCount) { }

		inline int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressed" << " keycode: " << m_keycode << " (" << m_repeatCount << " repeat counts)";
			return ss.str();
		}

		DEF_AEVENT_TYPE(KeyPressed);

	private:
		int m_repeatCount;
	};

	class KeyTypedEvent : public KeyEvent
	{
		public:
			KeyTypedEvent(int keycode) : KeyEvent(keycode) { }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyTyped char: " << (char) m_keycode;
				return ss.str();
			}

			DEF_AEVENT_TYPE(KeyTyped);
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleased keycode: " << m_keycode;
			return ss.str();
		}

		DEF_AEVENT_TYPE(KeyReleased);
	};

}