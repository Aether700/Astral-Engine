#pragma once
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	class WindowsTime : public Time
	{
	public:
		WindowsTime();

	protected:

		virtual double GetTimeImpl() override;
		virtual double GetDeltaTimeImpl() override;
		virtual double GetDeltaTimeMsImpl() override;
		virtual void UpdateTimeImpl() override;

	private:
		// number of system ticks per second
		std::int64_t m_frequency;

		// in system ticks
		std::int64_t m_appStartTime;
		std::int64_t m_currFrameTime;
		std::int64_t m_lastFrameTime;
	};
}