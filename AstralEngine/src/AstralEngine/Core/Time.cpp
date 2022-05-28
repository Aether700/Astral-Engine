#include "aepch.h"
#include "Time.h"

namespace AstralEngine
{
	// Timer /////////////////////////////////////////////////////////

	Timer::Timer(float time) : m_currTimer(0.0f), m_timer(time)
	{
		Time::AddTimer(this);
	}

	Timer::Timer(const Timer& other)
		: m_timer(other.m_timer), m_currTimer(other.m_currTimer), m_listeners(other.m_listeners)
	{
		Time::AddTimer(this);
	}

	Timer::~Timer()
	{
		Time::RemoveTimer(this);
	}

	/*Sets the time to wait between a reset and the timer being ready.
	  Note that this function does not reset the timer and changing the time
	  of the timer might change it's ready state

	  the time provided will be ignored if it is <= 0.0f
	*/
	void Timer::SetTimer(float time)
	{
		if (time > 0.0f)
		{
			m_timer = time;
		}
	}

	// retrives the time to wait between a reset and the timer being ready
	float Timer::GetTime() const
	{
		return m_timer;
	}

	// checks if the timer has waited the provided amount of time since it was last reset
	bool Timer::IsReady()
	{
		return m_currTimer >= m_timer;
	}

	// resets the timer so it can countdown again
	void Timer::Reset()
	{
		m_currTimer = 0.0f;
	}

	void Timer::SetAsReady()
	{
		m_currTimer = m_timer;
	}

	// add/remove a listener to be notified when the timer is ready.
	void Timer::AddListener(ADelegate<void()> listener)
	{
		m_listeners.AddDelegate(listener);
	}

	void Timer::RemoveListener(ADelegate<void()> listener)
	{
		m_listeners.RemoveDelegate(listener);
	}

	void Timer::SetActive(bool val) { m_isActive = val; }

	bool Timer::IsActive() const { return m_isActive; }

	Timer& Timer::operator=(const Timer& other)
	{
		m_timer = other.m_timer;
		m_currTimer = other.m_currTimer;
		m_listeners = other.m_listeners;
		return *this;
	}

	void Timer::OnUpdate()
	{
		if (m_isActive)
		{
			if (m_currTimer < m_timer)
			{
				m_currTimer += Time::GetDeltaTime();
				if (IsReady())
				{
					m_listeners();
				}
			}
		}
	}
}