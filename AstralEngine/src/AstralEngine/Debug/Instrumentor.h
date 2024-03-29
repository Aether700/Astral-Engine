#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>
#include "AstralEngine/Core/Core.h"

#ifdef AE_PROFILE
	#define AE_PROFILE_BEGIN_SESSION(name, filepath) ::AstralEngine::Instrumentor::BeginSession(name, filepath)
	#define AE_PROFILE_END_SESSION() ::AstralEngine::Instrumentor::EndSession()
	#define AE_PROFILE_SCOPE(name) ::AstralEngine::ATimer timer##__LINE__(name)
	#define AE_PROFILE_FUNCTION() AE_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define AE_PROFILE_BEGIN_SESSION(name, filepath)
	#define AE_PROFILE_END_SESSION()
	#define AE_PROFILE_SCOPE(name)
	#define AE_PROFILE_FUNCTION()
#endif

//pass json files to "chrome://tracing" to see data
namespace AstralEngine
{
	struct ProfileResult
	{
		std::string name;
		long long start, end;
		unsigned int threadID;
	};

	struct InstrumentationSession
	{
		std::string name;
		InstrumentationSession(const std::string& n) : name(n) { }
	};

	class Instrumentor
	{
	public:

		static void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			Get().m_outputStream.open(filepath);
			Get().WriteHeader();
			Get().m_currentSession = new InstrumentationSession(name);
		}

		static void EndSession()
		{
			Get().WriteFooter();
			Get().m_outputStream.close();
			delete Get().m_currentSession;
			Get().m_profileCount = 0;
		}

		static void WriteProfile(const ProfileResult& result)
		{
			if (Get().m_profileCount > 0)
			{
				Get().m_outputStream << ",";
			}
			Get().m_profileCount++;

			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			Get().m_outputStream << "{\n";
			Get().m_outputStream << "\"cat\":\"function\",\n";
			Get().m_outputStream << "\"dur\":" << (result.end - result.start) << ",\n";
			Get().m_outputStream << "\"name\":\"" << name << "\",\n";
			Get().m_outputStream << "\"ph\":\"X\",\n";
			Get().m_outputStream << "\"pid\":0,\n";
			Get().m_outputStream << "\"tid\":" << result.threadID << ",\n";
			Get().m_outputStream << "\"ts\":" << result.start << "\n";
			Get().m_outputStream << "}\n";

			Get().m_outputStream.flush();
		}

		static void WriteHeader()
		{
			Get().m_outputStream << "[\n";
			Get().m_outputStream.flush();
		}

		static void WriteFooter()
		{
			Get().m_outputStream << "]";
			Get().m_outputStream.flush();
		}


	private:
		Instrumentor() : m_currentSession(nullptr), m_profileCount(0) { }
		
		static Instrumentor& Get()
		{
			static Instrumentor* instance = new Instrumentor();
			return *instance;
		}

		InstrumentationSession* m_currentSession;
		std::ofstream m_outputStream;
		int m_profileCount;
	};

	class ATimer
	{
	public:
		ATimer(const char* name) : m_name(name), m_stopped(false)
		{
			m_startPoint = std::chrono::high_resolution_clock::now();
		}

		~ATimer()
		{
			if (!m_stopped)
			{
				Stop();
			}
		}

		void Stop()
		{
			auto endTime = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

			unsigned int threadID = (unsigned int)std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::WriteProfile({ m_name, start, end, threadID });

			m_stopped = true;
		}

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::steady_clock> m_startPoint;
		bool m_stopped;
	};
}