#include "aepch.h"
#include "Input.h"

namespace AstralEngine
{
	// InputDevice //////////////////////////////////////////////////////////////////
	InputDevice::~InputDevice() { }

	void InputDevice::OnConnect() const { m_onConnect(this); }
	
	void InputDevice::OnDisconnect() const { m_onDisconnect(this); }
	
	void InputDevice::AddListenerOnConnect(const ADelegate<void(const InputDevice*)>& listener) 
	{ 
		m_onConnect.AddDelegate(listener); 
	}

	void InputDevice::AddListenerOnDisconnect(const ADelegate<void(const InputDevice*)>& listener)
	{
		m_onDisconnect.AddDelegate(listener);
	}
	
	void InputDevice::RemoveListenerOnConnect(const ADelegate<void(const InputDevice*)>& listener)
	{
		m_onConnect.RemoveDelegate(listener);
	}
	
	void InputDevice::RemoveListenerOnDisconnect(const ADelegate<void(const InputDevice*)>& listener)
	{
		m_onDisconnect.RemoveDelegate(listener);
	}

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

	bool Keyboard::GetKeyDown(KeyCode key) const
	{
		if (!m_keyStates.ContainsKey(key))
		{
			return false;
		}
		return m_keyStates[key].wasJustPressed;
	}
	
	bool Keyboard::GetAnyKey() const { return m_numKeysDown > 0; }

	bool Keyboard::GetAnyKeyDown() const { return m_anyJustPressed; }

	void Keyboard::OnUpdate()
	{
		m_anyJustPressed = false;
		for (KeyCode key : m_toUpdate)
		{
			m_keyStates[key].wasJustPressed = false;
		}
		m_toUpdate.Clear();
	}

	void Keyboard::OnEvent(AEvent& e)
	{
		KeyPressedEvent* pressed = dynamic_cast<KeyPressedEvent*>(&e);
		if (pressed != nullptr)
		{
			KeyCode key = pressed->GetKeyCode();
			if (m_keyStates.ContainsKey(key))
			{
				KeyState& state = m_keyStates[key];
				if (state.isDown)
				{
					m_numKeysDown--;
				}
				state.isDown = true;
				state.wasJustPressed = true;
			}
			else
			{
				KeyState state;
				state.isDown = true;
				state.wasJustPressed = true;
				m_keyStates[key] = state;
			}
			m_toUpdate.Add(key);
			m_numKeysDown++;
		}
		else
		{
			KeyReleasedEvent* released = dynamic_cast<KeyReleasedEvent*>(&e);
			if (released != nullptr)
			{
				KeyCode key = released->GetKeyCode();

				if (m_keyStates.ContainsKey(key))
				{
					KeyState& state = m_keyStates[key];
					state.isDown = false;
					state.wasJustPressed = false;
				}
				else
				{
					m_keyStates[key] = KeyState();
				}
			}
			else 
			{
				WindowLostFocusEvent* lostFocus = dynamic_cast<WindowLostFocusEvent*>(&e);
				if (lostFocus != nullptr)
				{
					Clear();
				}
			}
		}
	}

	// clears the key states of the keyboard
	void Keyboard::Clear()
	{
		for(AKeyElementPair<KeyCode, KeyState>& pair : m_keyStates)
		{
			pair.GetElement().Reset();
		}
		m_anyJustPressed = false;
		m_numKeysDown = 0;
	}

	Keyboard::KeyState::KeyState() : isDown(false), wasJustPressed(false) { }

	void Keyboard::KeyState::Reset() 
	{
		isDown = false;
		wasJustPressed = false;
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

	bool Mouse::GetButtonDown(MouseButtonCode button) const
	{
		if (!m_buttonStates.ContainsKey(button))
		{
			return false;
		}
		return m_buttonStates[button].wasJustPressed;
	}

	bool Mouse::GetAnyButton(MouseButtonCode button) const { return m_numButtonsDown > 0; }

	bool Mouse::GetAnyButtonDown(MouseButtonCode button) const { return m_anyJustPressed; }

	const Vector2Int& Mouse::GetMousePos() const { return m_mousePos;  }
	
	int Mouse::GetMousePosX() const { return m_mousePos.x; }
	
	int Mouse::GetMousePosY() const { return m_mousePos.y; }

	void Mouse::Clear()
	{
		for (AKeyElementPair<MouseButtonCode, MouseButtonState>& pair : m_buttonStates)
		{
			pair.GetElement().Reset();
		}
		m_anyJustPressed = false;
		m_numButtonsDown = 0;
	}

	void Mouse::OnUpdate()
	{
		for (MouseButtonCode button : m_toUpdate)
		{
			m_buttonStates[button].wasJustPressed = false;
		}
		m_toUpdate.Clear();
	}

	void Mouse::OnEvent(AEvent& e)
	{
		MouseMovedEvent* moved = dynamic_cast<MouseMovedEvent*>(&e);
		if (moved != nullptr)
		{
			m_mousePos.x = moved->GetXPos();
			m_mousePos.y = moved->GetYPos();
		}
		else
		{
			MouseButtonPressedEvent* pressed = dynamic_cast<MouseButtonPressedEvent*>(&e);
			if (pressed != nullptr)
			{
				MouseButtonCode button = pressed->GetButtonKeycode();
				if (m_buttonStates.ContainsKey(button))
				{
					m_buttonStates[button].isDown = true;
					m_buttonStates[button].wasJustPressed = true;
				}
				else
				{
					MouseButtonState state;
					state.isDown = true;
					state.wasJustPressed = true;
					m_buttonStates[button] = state;
				}
				m_numButtonsDown++;
				m_toUpdate.Add(button);
			}
			else
			{
				MouseButtonReleasedEvent* released = dynamic_cast<MouseButtonReleasedEvent*>(&e);
				if (released != nullptr)
				{
					MouseButtonCode button = released->GetButtonKeycode();
					if (m_buttonStates.ContainsKey(button))
					{
						m_buttonStates[button].isDown = false;
						m_buttonStates[button].wasJustPressed = false;
					}
					else
					{
						MouseButtonState state;
						m_buttonStates[button] = state;
					}
					m_numButtonsDown--;
				}
				else if (dynamic_cast<WindowLostFocusEvent*>(&e) != nullptr)
				{
					Clear();
				}
			}
		}
	}

	Mouse::MouseButtonState::MouseButtonState() : isDown(false), wasJustPressed(false) { }
	
	void Mouse::MouseButtonState::Reset()
	{
		isDown = false;
		wasJustPressed = false;
	}
}