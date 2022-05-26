#pragma once
#include "AstralEngine/Debug/Instrumentor.h"
#include <cstdlib>
#include <windows.h>
#include <fstream>
#include <sstream>


#ifdef AE_DEBUG
	#define AE_ENABLE_CORE_LOG
#endif

#ifdef AE_ENABLE_CORE_LOG
	#define AE_CORE_INFO(...) AstralEngine::Logger::Info("[ASTRAL_ENGINE] ", __VA_ARGS__)
	#define AE_CORE_WARN(...) AstralEngine::Logger::Warn("[ASTRAL_ENGINE] ", __VA_ARGS__)
	#define AE_CORE_ERROR(...) AstralEngine::Logger::Error("[ASTRAL_ENGINE] ", __VA_ARGS__)
#else
	#define AE_CORE_INFO(...)
	#define AE_CORE_WARN(...)
	#define AE_CORE_ERROR(...)
#endif

#define AE_INFO(...) AstralEngine::Logger::Info("[APP] ", __VA_ARGS__)
#define AE_WARN(...) AstralEngine::Logger::Warn("[APP] ", __VA_ARGS__)
#define AE_ERROR(...) AstralEngine::Logger::Error("[APP] ", __VA_ARGS__)


namespace AstralEngine
{
	//https://fmt.dev/latest/api.html#formatting-user-defined-types
	class Logger
	{
	public:
		static void Init(const char* filepath = "AstralEngine.log");

		static void Info(const std::string& prefix, const std::string format, ...);
		static void Warn(const std::string& prefix, const std::string format, ...);
		static void Error(const std::string& prefix, const std::string format, ...);

	private:
		static std::string GetFormatedString(const std::string& format, va_list args);
		static void PrintInColor(const std::string& prefix, const std::string& message, WORD color);

		static HANDLE s_handle;
		static std::ofstream s_file;
	};
}

