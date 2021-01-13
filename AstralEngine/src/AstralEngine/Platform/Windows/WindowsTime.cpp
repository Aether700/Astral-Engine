#include "aepch.h"
#include "WindowsTime.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	AReference<Time> Time::s_instance = AReference<WindowsTime>::Create();

	WindowsTime::WindowsTime() : m_currFrameTime(0.0f), m_lastFrameTime(0.0f) { }

	float WindowsTime::DeltaTimeImpl()
	{
		return m_currFrameTime;
	}

	void WindowsTime::UpdateTimeImpl() 
	{
		float temp = (float) glfwGetTime();
		m_currFrameTime = temp - m_lastFrameTime;
		m_lastFrameTime = temp;
	}
}