#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Core/MouseButtonCodes.h"
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"

namespace AstralEngine
{
	class InputDevice
	{
	public:
		virtual ~InputDevice();

		void OnConnect() const;
		void OnDisconnect() const;

		void AddListenerOnConnect(const ADelegate<void(const InputDevice*)>& listener);
		void AddListenerOnDisconnect(const ADelegate<void(const InputDevice*)>& listener);

		void RemoveListenerOnConnect(const ADelegate<void(const InputDevice*)>& listener);
		void RemoveListenerOnDisconnect(const ADelegate<void(const InputDevice*)>& listener);

		virtual void OnUpdate() { }

		virtual void OnEvent(AEvent& e) = 0;

	private:
		SignalHandler<void(const InputDevice*)> m_onDisconnect;
		SignalHandler<void(const InputDevice*)> m_onConnect;
	};

	class Keyboard final : public InputDevice
	{
	public:
		Keyboard();

		bool GetKey(KeyCode key) const;
		bool GetKeyDown(KeyCode key) const;
		bool GetAnyKey() const;
		bool GetAnyKeyDown() const;


		virtual void OnUpdate() override;
		virtual void OnEvent(AEvent& e) override;

		// clears the key states of the keyboard
		void Clear();

	private:
		struct KeyState
		{
			bool isDown;
			bool wasJustPressed;

			KeyState();

			void Reset();
		};

		AUnorderedMap<KeyCode, KeyState> m_keyStates;
		ASinglyLinkedList<KeyCode> m_toUpdate;
		bool m_anyJustPressed;
		size_t m_numKeysDown;
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
		
		static Vector2 GetMousePosition();
		static float GetMouseXPos();
		static float GetMouseYPos();

	private:
		static void OnUpdate();
		static void OnEvent(AEvent& e)
		{
			s_keyboard.OnEvent(e);
		}
		static bool OnKeyPressedEvent(KeyPressedEvent& keyPressed);
		static bool OnKeyReleasedEvent(KeyReleasedEvent& keyReleased);
		static bool OnMousePressedEvent(MouseButtonPressedEvent& mousePressed);
		static bool OnMouseReleasedEvent(MouseButtonReleasedEvent& mouseReleased);

		static Keyboard s_keyboard;
	};

}