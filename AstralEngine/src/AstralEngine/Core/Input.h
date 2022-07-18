#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Core/MouseButtonCodes.h"
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"

namespace AstralEngine
{
	class Keyboard final
	{
		friend class Input;
	public:
		bool GetKey(KeyCode key) const;
		bool GetKeyUp(KeyCode key) const;
		bool GetKeyDown(KeyCode key) const;

		bool GetAnyKey() const;
		bool GetAnyKeyUp() const;
		bool GetAnyKeyDown() const;

		void OnUpdate();
		void OnEvent(AEvent& e);

		// clears the key states of the keyboard
		void Clear();

	private:
		struct KeyState
		{
			bool isDown;
			bool wasDownLastFrame;

			KeyState();

			void Reset();
		};

		Keyboard();

		bool OnKeyPressedEvent(KeyPressedEvent& pressed);
		bool OnKeyReleasedEvent(KeyReleasedEvent& released);
		bool OnWindowLostFocusEvent(WindowLostFocusEvent& lostFocus);

		AUnorderedMap<KeyCode, KeyState> m_keyStates;
		ASinglyLinkedList<KeyCode> m_toUpdate;
		bool m_anyJustPressed;
		bool m_anyJustReleased;
		size_t m_numKeysDown;
	};

	class Mouse final
	{
		friend class Input;
	public:
		bool GetButton(MouseButtonCode button) const;
		bool GetButtonUp(MouseButtonCode button) const;
		bool GetButtonDown(MouseButtonCode button) const;

		bool GetAnyButton(MouseButtonCode button) const;
		bool GetAnyButtonUp(MouseButtonCode button) const;
		bool GetAnyButtonDown(MouseButtonCode button) const;

		const Vector2Int& GetPos() const;
		int GetPosX() const;
		int GetPosY() const;

		void OnUpdate();
		void OnEvent(AEvent& e);

		// clears the button states of the mouse
		void Clear();

	private:
		struct MouseButtonState
		{
			bool isDown;
			bool wasDownLastFrame;

			MouseButtonState();
			void Reset();
		};

		Mouse();

		bool OnMouseMovedEvent(MouseMovedEvent& moved);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& pressed);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& released);
		bool OnWindowLostFocusEvent(WindowLostFocusEvent& lostFocus);

		Vector2Int m_mousePos;
		AUnorderedMap<MouseButtonCode, MouseButtonState> m_buttonStates;
		ASinglyLinkedList<MouseButtonCode> m_toUpdate;
		bool m_anyJustPressed;
		bool m_anyJustReleased;
		size_t m_numButtonsDown;
	};

	class Input
	{
		friend class Application;
	public:
		static bool GetKey(KeyCode key);
		static bool GetAnyKey();
		
		static bool GetKeyDown(KeyCode key);
		static bool GetAnyKeyDown();
		
		static bool GetMouseButton(MouseButtonCode mouseButton);
		static bool GetMouseButtonDown(MouseButtonCode mouseButton);
		
		static Vector2Int GetMousePosition();
		static int GetMouseXPos();
		static int GetMouseYPos();

		static Keyboard& GetKeyboard();
		static Mouse& GetMouse();

	private:
		static void OnUpdate();
		static void OnEvent(AEvent& e);

		static Keyboard s_keyboard;
		static Mouse s_mouse;
	};

}