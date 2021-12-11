#pragma once
#include "AstralEngine/Core/Time.h"

namespace AstralEngine
{
	class WindowsTime : public Time
	{
	public:
		WindowsTime();

	protected:

		virtual float GetTimeImpl() override;
		virtual float GetDeltaTimeImpl() override;
		virtual void UpdateTimeImpl() override;

	private:
		float m_lastFrameTime;
		float m_currFrameTime;
	};
}