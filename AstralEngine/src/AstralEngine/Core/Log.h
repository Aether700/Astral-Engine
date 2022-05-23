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

		static void Init(const char* filepath = "AstralEngine.log")
		{
			s_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			s_file = std::ofstream(filepath);
		}

		static void Info(const std::string& prefix, const std::string format, ...)
		{
			AE_PROFILE_FUNCTION();

			if (format == "")
			{
				return;
			}

			va_list args;
			va_start(args, format);
			
			std::string message = GetFormatedString(format, args);
			PrintInColor(prefix, message, FOREGROUND_INTENSITY | FOREGROUND_GREEN);

			va_end(args);
		}

		static void Warn(const std::string& prefix, const std::string format, ...)
		{
			AE_PROFILE_FUNCTION();
			
			if (format == "")
			{
				return;
			}

			va_list args;
			va_start(args, format);

			std::string message = GetFormatedString(format, args);
			PrintInColor(prefix, message, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

			va_end(args);
		}

		static void Error(const std::string& prefix, const std::string format, ...)
		{
			AE_PROFILE_FUNCTION();

			if (format == "")
			{
				return;
			}

			va_list args;
			va_start(args, format);

			std::string message = GetFormatedString(format, args);
			PrintInColor(prefix, message, FOREGROUND_INTENSITY | FOREGROUND_RED);

			va_end(args);
			__debugbreak();
		}

	private:

		static std::string GetFormatedString(const std::string& format, va_list args)
		{
			AE_PROFILE_FUNCTION();
			std::stringstream ss;

			for (size_t i = 0; i < format.length(); i++)
			{
				if (format[i] == '%')
				{
					double temp;
					bool correctFormat = true;
					switch (format[i + 1])
					{
					case 'c':
						ss << va_arg(args, char);
						break;

					case 'd':
					case 'i':
						ss << va_arg(args, int);
						break;

					case 'u':
						ss << va_arg(args, size_t);
						break;

					case 'f':
						temp = va_arg(args, double);
						ss << temp;
						break;

					case 's':
						ss << va_arg(args, const char*);
						break;

					case 'S':
						ss << va_arg(args, std::string);
						break;

					default:
						correctFormat = false;
					}

					if (correctFormat)
					{
						i++;
						continue;
					}
				}

				ss << format[i];
			}

			return ss.str();
		}

		static void PrintInColor(const std::string& prefix, const std::string& message, WORD color)
		{
			SetConsoleTextAttribute(s_handle, color);
			s_file << prefix << message << "\n";
			std::cout << prefix << message << "\n";
			std::cout.flush();
			SetConsoleTextAttribute(s_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		static HANDLE s_handle;
		static std::ofstream s_file;
	};
}

