#pragma once
#include "AstralEngine/Debug/Instrumentor.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Data Struct/ADelegate.h"
#include "AstralEngine/Data Struct/ADoublyLinkedList.h"
#include "AstralEngine/Math/Utils.h"

namespace AstralEngine
{
	// Timer class used to measure amounts of time such as when waiting for a certain delay between inputs
	class Timer
	{
		friend class Time;
	public:
		Timer(float time = 1.0f);
		Timer(const Timer& other);
		~Timer();

		/*Sets the time to wait between a reset and the timer being ready.
		  Note that this function does not reset the timer and changing the time
		  of the timer might change it's ready state

		  the time provided will be ignored if it is <= 0.0f
		*/
		void SetTimer(float time);

		// retrives the time to wait between a reset and the timer being ready
		float GetTime() const;

		// checks if the timer has waited the provided amount of time since it was last reset
		bool IsReady();

		// resets the timer so it can countdown again
		void Reset();
		// sets the timer as ready
		void SetAsReady();

		// add/remove a listener to be notified when the timer is ready.
		void AddListener(ADelegate<void()> listener);
		void RemoveListener(ADelegate<void()> listener);

		// dictates whether or not the timer should be updated
		void SetActive(bool val);
		bool IsActive() const;

		Timer& operator=(const Timer& other);

	private:
		void OnUpdate();

		bool m_isActive;
		float m_timer;
		float m_currTimer;
		SignalHandler<void()> m_listeners;
	};

	class Time
	{
		friend class Timer;
	public:
		virtual ~Time() { }

		// returns the time since the application started in seconds
		static double GetTime() 
		{
			return s_instance->GetTimeImpl();
		}


		static void UpdateTime() 
		{
			s_instance->UpdateTimeImpl();
			UpdateTimers();
		}
		
		static double GetDeltaTime() 
		{
			return s_instance->GetDeltaTimeImpl(); 
		}

		static double GetDeltaTimeMs() 
		{
			return s_instance->GetDeltaTimeMsImpl();
		}

	protected:
		virtual double GetDeltaTimeImpl() = 0;
		virtual double GetDeltaTimeMsImpl() = 0;
		virtual void UpdateTimeImpl() = 0;

		virtual double GetTimeImpl() = 0;

	private:
		static void AddTimer(Timer* t)
		{
			s_instance->m_timers.Add(t);
		}

		static void RemoveTimer(Timer* t)
		{
			s_instance->m_timers.Remove(t);
		}

		static void UpdateTimers()
		{
			for(Timer* t : s_instance->m_timers)
			{
				t->OnUpdate();
			}
		}

		ASinglyLinkedList<Timer*> m_timers;
		static AReference<Time> s_instance;
	};
}