#pragma once
#include "AEvent.h"
#include "AstralEngine/Core/Keycodes.h"
#include <sstream>

namespace AstralEngine
{
	class KeyEvent : public AEvent
	{
	public:
		inline KeyCode GetKeyCode() const { return m_keycode; }

	protected:
		KeyEvent(KeyCode keycode) : m_keycode(keycode) { }

		DEF_AEVENT_CATEGORY((AEventCategory) (AEventCategory::AEventCategoryInput | AEventCategory::AEventCategoryKeyboard));

		KeyCode m_keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode) : KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressed" << " keycode: " << (int)m_keycode;
			return ss.str();
		}

		DEF_AEVENT_TYPE(KeyPressed);
	};

	class KeyRepeatedEvent : public KeyEvent
	{
	public:
		KeyRepeatedEvent(KeyCode keycode) : KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyRepeated" << " keycode: " << (int)m_keycode;
			return ss.str();
		}

		DEF_AEVENT_TYPE(KeyRepeated);
	};

	class KeyTypedEvent : public KeyEvent
	{
		public:
			KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) { }

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
		KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleased keycode: " << (int)m_keycode;
			return ss.str();
		}

		DEF_AEVENT_TYPE(KeyReleased);
	};

}