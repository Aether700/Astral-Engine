#include "aepch.h"
#include "Input.h"

//temp
#include "Application.h"
///

namespace AstralEngine
{
	// Keyboard ///////////////////////////////////////////////////

	Keyboard::Keyboard() : m_anyJustPressed(false), m_numKeysDown(0) { }

	bool Keyboard::GetKey(KeyCode key) const
	{
		if (!m_keyStates.ContainsKey(key))
		{
			return false;
		}
		return m_keyStates[key].isDown;
	}

	bool Keyboard::GetKeyUp(KeyCode key) const
	{
		if (!m_keyStates.ContainsKey(key))
		{
			return false;
		}
		const KeyState& state = m_keyStates[key];
		return state.wasDownLastFrame && !state.isDown;
	}

	bool Keyboard::GetKeyDown(KeyCode key) const
	{
		if (!m_keyStates.ContainsKey(key))
		{
			return false;
		}
		const KeyState& state = m_keyStates[key];
		return !state.wasDownLastFrame && state.isDown;
	}
	
	bool Keyboard::GetAnyKey() const { return m_numKeysDown > 0; }
	bool Keyboard::GetAnyKeyUp() const { return m_anyJustReleased; }
	bool Keyboard::GetAnyKeyDown() const { return m_anyJustPressed; }

	void Keyboard::OnUpdate()
	{
		m_anyJustPressed = false;
		m_anyJustReleased = false;
		for (KeyCode key : m_toUpdate)
		{
			m_keyStates[key].wasDownLastFrame = !m_keyStates[key].wasDownLastFrame;
		}
		m_toUpdate.Clear();
	}

	void Keyboard::OnEvent(AEvent& e)
	{
		AEventDispatcher dispatcher = AEventDispatcher(e);
		dispatcher.HandleAEvent<KeyPressedEvent>(ADelegate<bool(KeyPressedEvent&)>
			(function<&Keyboard::OnKeyPressedEvent>, this));
		dispatcher.HandleAEvent<KeyReleasedEvent>(ADelegate<bool(KeyReleasedEvent&)>
			(function<&Keyboard::OnKeyReleasedEvent>, this));
		dispatcher.HandleAEvent<WindowLostFocusEvent>(ADelegate<bool(WindowLostFocusEvent&)>
			(function<&Keyboard::OnWindowLostFocusEvent>, this));
	}

	// clears the key states of the keyboard
	void Keyboard::Clear()
	{
		for(AKeyElementPair<KeyCode, KeyState>& pair : m_keyStates)
		{
			pair.GetElement().Reset();
		}
		m_anyJustPressed = false;
		m_anyJustReleased = false;
		m_numKeysDown = 0;
	}

	bool Keyboard::OnKeyPressedEvent(KeyPressedEvent& pressed)
	{
		KeyCode key = pressed.GetKeyCode();
		if (m_keyStates.ContainsKey(key))
		{
			KeyState& state = m_keyStates[key];
			state.isDown = true;
			state.wasDownLastFrame = false;
		}
		else
		{
			KeyState state;
			state.isDown = true;
			state.wasDownLastFrame = false;
			m_keyStates[key] = state;
		}
		m_toUpdate.Add(key);
		m_numKeysDown++;
		m_anyJustPressed = true;
		return false;
	}

	bool Keyboard::OnKeyReleasedEvent(KeyReleasedEvent& released)
	{
		KeyCode key = released.GetKeyCode();

		if (m_keyStates.ContainsKey(key))
		{
			KeyState& state = m_keyStates[key];
			state.isDown = false;
			m_numKeysDown--;
		}
		else
		{
			KeyState state;
			state.wasDownLastFrame = true;
			m_keyStates[key] = state;
		}
		m_toUpdate.Add(key);
		m_anyJustReleased = true;
		return false;
	}

	bool Keyboard::OnWindowLostFocusEvent(WindowLostFocusEvent& lostFocus)
	{
		Clear();
		return false;
	}

	Keyboard::KeyState::KeyState() : isDown(false), wasDownLastFrame(false) { }

	void Keyboard::KeyState::Reset() 
	{
		isDown = false;
		wasDownLastFrame = false;
	}

	// Mouse /////////////////////////////////////////////////////////////////

	Mouse::Mouse() : m_anyJustPressed(false), m_numButtonsDown(0)
	{
		POINT mousePos;
		if (GetCursorPos(&mousePos) == 0)
		{
			AE_CORE_WARN("Could not retrieve initial mouse position");
		}
		m_mousePos = Vector2Int(mousePos.x, mousePos.y);
	}

	bool Mouse::GetButton(MouseButtonCode button) const
	{
		if (!m_buttonStates.ContainsKey(button))
		{
			return false;
		}
		return m_buttonStates[button].isDown;
	}

	bool Mouse::GetButtonUp(MouseButtonCode button) const
	{
		if (!m_buttonStates.ContainsKey(button))
		{
			return false;
		}
		const MouseButtonState& state = m_buttonStates[button];
		return !state.isDown && state.wasDownLastFrame;
	}

	bool Mouse::GetButtonDown(MouseButtonCode button) const
	{
		if (!m_buttonStates.ContainsKey(button))
		{
			return false;
		}
		const MouseButtonState& state = m_buttonStates[button];
		return state.isDown && !state.wasDownLastFrame;
	}

	bool Mouse::GetAnyButton(MouseButtonCode button) const { return m_numButtonsDown > 0; }
	bool Mouse::GetAnyButtonUp(MouseButtonCode button) const { return m_anyJustReleased; }
	bool Mouse::GetAnyButtonDown(MouseButtonCode button) const { return m_anyJustPressed; }

	const Vector2Int& Mouse::GetPos() const { return m_mousePos;  }
	
	int Mouse::GetPosX() const { return m_mousePos.x; }
	
	int Mouse::GetPosY() const { return m_mousePos.y; }

	void Mouse::OnUpdate()
	{
		m_anyJustPressed = false;
		m_anyJustReleased = false;
		for (MouseButtonCode button : m_toUpdate)
		{
			m_buttonStates[button].wasDownLastFrame = !m_buttonStates[button].wasDownLastFrame;
		}
		m_toUpdate.Clear();
	}

	void Mouse::OnEvent(AEvent& e)
	{
		AEventDispatcher dispatcher = AEventDispatcher(e);
		dispatcher.HandleAEvent<MouseMovedEvent>(ADelegate<bool(MouseMovedEvent&)>
			(function<&Mouse::OnMouseMovedEvent>, this));
		dispatcher.HandleAEvent<MouseButtonPressedEvent>(ADelegate<bool(MouseButtonPressedEvent&)>
			(function<&Mouse::OnMouseButtonPressedEvent>, this));
		dispatcher.HandleAEvent<MouseButtonReleasedEvent>(ADelegate<bool(MouseButtonReleasedEvent&)>
			(function<&Mouse::OnMouseButtonReleasedEvent>, this));
		dispatcher.HandleAEvent<WindowLostFocusEvent>(ADelegate<bool(WindowLostFocusEvent&)>
			(function<&Mouse::OnWindowLostFocusEvent>, this));
	}

	void Mouse::Clear()
	{
		for (AKeyElementPair<MouseButtonCode, MouseButtonState>& pair : m_buttonStates)
		{
			pair.GetElement().Reset();
		}
		m_anyJustPressed = false;
		m_anyJustReleased = false;
		m_numButtonsDown = 0;
	}

	bool Mouse::OnMouseMovedEvent(MouseMovedEvent& moved)
	{
		m_mousePos.x = moved.GetXPos();
		m_mousePos.y = moved.GetYPos();
		return false;
	}

	bool Mouse::OnMouseButtonPressedEvent(MouseButtonPressedEvent& pressed)
	{
		MouseButtonCode button = pressed.GetButtonKeycode();
		if (m_buttonStates.ContainsKey(button))
		{
			m_buttonStates[button].isDown = true;
			m_buttonStates[button].wasDownLastFrame = false;
		}
		else
		{
			MouseButtonState state;
			state.isDown = true;
			m_buttonStates[button] = state;
		}
		m_numButtonsDown++;
		m_toUpdate.Add(button);
		return false;
	}

	bool Mouse::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& released)
	{
		MouseButtonCode button = released.GetButtonKeycode();
		if (m_buttonStates.ContainsKey(button))
		{
			m_buttonStates[button].isDown = false;
		}
		else
		{
			MouseButtonState state;
			state.wasDownLastFrame = true;
			m_buttonStates[button] = state;
		}
		m_numButtonsDown--;
		m_toUpdate.Add(button);
		return false;
	}

	bool Mouse::OnWindowLostFocusEvent(WindowLostFocusEvent& lostFocus)
	{
		Clear();
		return false;
	}

	Mouse::MouseButtonState::MouseButtonState() : isDown(false), wasDownLastFrame(false) { }
	
	void Mouse::MouseButtonState::Reset()
	{
		isDown = false;
		wasDownLastFrame = false;
	}

	// Input ///////////////////////////////////////////////////////
	
	Keyboard Input::s_keyboard;
	Mouse Input::s_mouse;

	bool Input::GetKey(KeyCode key) { return s_keyboard.GetKey(key); }
	bool Input::GetAnyKey() { return s_keyboard.GetAnyKey(); }

	bool Input::GetKeyDown(KeyCode key) { return s_keyboard.GetKeyDown(key); }
	bool Input::GetAnyKeyDown() { return s_keyboard.GetAnyKeyDown(); }

	bool Input::GetMouseButton(MouseButtonCode mouseButton) { return s_mouse.GetButton(mouseButton); }
	bool Input::GetMouseButtonDown(MouseButtonCode mouseButton) { return s_mouse.GetButtonDown(mouseButton); }

	Vector2Int Input::GetMousePosition() { return s_mouse.GetPos(); }
	int Input::GetMouseXPos() { return s_mouse.GetPosX(); }
	int Input::GetMouseYPos() { return s_mouse.GetPosY(); }

	Keyboard& Input::GetKeyboard() { return s_keyboard; }
	Mouse& Input::GetMouse() { return s_mouse; }

	void Input::OnUpdate()
	{
		s_keyboard.OnUpdate();
		s_mouse.OnUpdate();
	}
	
	void Input::OnEvent(AEvent& e)
	{
		s_keyboard.OnEvent(e);
		s_mouse.OnEvent(e);
	}

}