#include "aepch.h"
#include "WindowsTime.h"

#include <GLFW/glfw3.h>

namespace AstralEngine
{
	AReference<Time> Time::s_instance = AReference<WindowsTime>::Create();

	WindowsTime::WindowsTime() 
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
		QueryPerformanceCounter((LARGE_INTEGER*)&m_appStartTime);
	}

	double WindowsTime::GetTimeImpl()
	{
		//return (float)glfwGetTime();
		std::int64_t currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		return (double)(currTime - m_appStartTime) / (double)m_frequency;
		
	}

	double WindowsTime::GetDeltaTimeImpl()
	{
		return (double)m_currFrameTime / (double)m_frequency;
	}

	double WindowsTime::GetDeltaTimeMsImpl()
	{
		std::int64_t ticksInMs = m_currFrameTime * 1000000000;
		return (double)ticksInMs / (double)m_frequency;
	}

	void WindowsTime::UpdateTimeImpl() 
	{
		std::int64_t currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_currFrameTime = currTime - m_lastFrameTime;
		m_lastFrameTime = currTime;
	}
}