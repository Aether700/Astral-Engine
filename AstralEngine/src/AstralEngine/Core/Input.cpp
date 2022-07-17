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
					m_numKeysDown--;
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
}