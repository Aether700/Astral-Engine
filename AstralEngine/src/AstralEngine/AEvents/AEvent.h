#pragma once
#include <string>
#include <functional>
#include <iostream>
#include "AstralEngine/Data Struct/ADelegate.h"

namespace AstralEngine
{
	enum AEventCategory
	{
		None = 0,
		AEventCategoryApplication = 1 << 0,
		AEventCategoryInput = 1 << 1,
		AEventCategoryKeyboard = 1 << 2,
		AEventCategoryMouse = 1 << 3,
		AEventCategoryMouseButton = 1 << 4
	};

	enum class AEventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyRepeated, KeyTyped, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	#define DEF_AEVENT_CATEGORY(category) inline AEventCategory GetCategoryFlags() const override { return category; }

	#define DEF_AEVENT_TYPE(type) static AEventType GetStaticType() { return AEventType::##type; }\
									AEventType GetType() const override { return GetStaticType(); }\
									const std::string GetName() const override { return #type; }

	class AEvent
	{
		friend class AEventDispatcher;
	public:
		AEvent() : m_isHandled(false) { }
		virtual AEventType GetType() const = 0;
		virtual const std::string GetName() const = 0;
		virtual AEventCategory GetCategoryFlags() const = 0;
		virtual std::string ToString() const = 0;

		inline bool IsHandled() { return m_isHandled; }

		inline bool IsInCategory(AEventCategory category)
		{
			return GetCategoryFlags() & category;
		}

	protected:
		bool m_isHandled;
	};

	class AEventDispatcher
	{
		template<typename T> using AEventFn = ADelegate<bool(T&)>;
	public:
		AEventDispatcher(AEvent& event) : m_event(event) { }

		template<typename T> bool HandleAEvent(AEventFn<T> function)
		{
			if (m_event.GetType() == T::GetStaticType())
			{
				m_event.m_isHandled = function((T&)m_event);
				return m_event.m_isHandled;
			}
			return false;
		}

	private:
		AEvent& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os, const AEvent& e)
	{
		return os << e.ToString();
	}
}