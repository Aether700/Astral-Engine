#include "aepch.h"
#include "WindowsTime.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	AReference<Time> Time::s_instance = AReference<WindowsTime>::Create();

	WindowsTime::WindowsTime() : m_currFrameTime(0.0f), m_lastFrameTime(0.0f) { }

	float WindowsTime::GetTimeImpl()
	{
		return (float)glfwGetTime();
	}

	float WindowsTime::GetDeltaTimeImpl()
	{
		return m_currFrameTime;
	}

	void WindowsTime::UpdateTimeImpl() 
	{
		float temp = GetTimeImpl();
		m_currFrameTime = temp - m_lastFrameTime;
		m_lastFrameTime = temp;
	}
}