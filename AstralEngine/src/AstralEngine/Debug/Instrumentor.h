#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>
#include "AstralEngine/Core/Core.h"

#ifdef AE_PROFILE
	#define AE_PROFILE_BEGIN_SESSION(name, filepath) ::AstralEngine::Instrumentor::Get().BeginSession(name, filepath)
	#define AE_PROFILE_END_SESSION() ::AstralEngine::Instrumentor::Get().EndSession()
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
		Instrumentor() : m_currentSession(nullptr), m_profileCount(0) { }

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			m_outputStream.open(filepath);
			WriteHeader();
			m_currentSession = new InstrumentationSession(name);
		}

		void EndSession()
		{
			WriteFooter();
			m_outputStream.close();
			delete m_currentSession;
			m_profileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (m_profileCount > 0)
			{
				m_outputStream << ",";
			}
			m_profileCount++;

			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_outputStream << "{\n";
			m_outputStream << "\"cat\":\"function\",\n";
			m_outputStream << "\"dur\":" << (result.end - result.start) << ",\n";
			m_outputStream << "\"name\":\"" << name << "\",\n";
			m_outputStream << "\"ph\":\"X\",\n";
			m_outputStream << "\"pid\":0,\n";
			m_outputStream << "\"tid\":" << result.threadID << ",\n";
			m_outputStream << "\"ts\":" << result.start << "\n";
			m_outputStream << "}\n";

			m_outputStream.flush();
		}

		void WriteHeader()
		{
			m_outputStream << "[\n";//"{\"other data\": {}, \"trace events\":[";
			m_outputStream.flush();
		}

		void WriteFooter()
		{
			m_outputStream << "]";//"]}";
			m_outputStream.flush();
		}

		static Instrumentor& Get()
		{
			static Instrumentor* instance = new Instrumentor();
			return *instance;
		}

	private:
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
			Instrumentor::Get().WriteProfile({ m_name, start, end, threadID });

			m_stopped = true;
		}

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::steady_clock> m_startPoint;
		bool m_stopped;
	};
}