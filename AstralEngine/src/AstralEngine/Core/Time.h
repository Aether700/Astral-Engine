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
		
		static float DeltaTime() 
		{
			return s_instance->DeltaTimeImpl(); 
		}

		static float DeltaTimeMs() 
		{
			return s_instance->DeltaTimeMsImpl();
		}

	protected:
		virtual float DeltaTimeImpl() = 0;
		
		virtual float DeltaTimeMsImpl() 
		{
			return s_instance->DeltaTimeImpl() * 1000.0f; 
		}
		
		virtual void UpdateTimeImpl() = 0;

		virtual float GetTimeImpl() = 0;

	private:
		static AReference<Time> s_instance;
	};
}