#pragma once
#include "AstralEngine/Debug/Instrumentor.h"
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	class Time
	{
	public:
		virtual ~Time() { }

		static float GetTime() 
		{
			return s_instance->GetTimeImpl();
		}


		static void UpdateTime() 
		{
			return s_instance->UpdateTimeImpl();
		}
		
		static float GetDeltaTime() 
		{
			return s_instance->GetDeltaTimeImpl(); 
		}

		static float GetDeltaTimeMs() 
		{
			return s_instance->GetDeltaTimeMsImpl();
		}

	protected:
		virtual float GetDeltaTimeImpl() = 0;
		
		virtual float GetDeltaTimeMsImpl() 
		{
			return s_instance->GetDeltaTimeImpl() * 1000.0f; 
		}
		
		virtual void UpdateTimeImpl() = 0;

		virtual float GetTimeImpl() = 0;

	private:
		static AReference<Time> s_instance;
	};
}