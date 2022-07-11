#include "aepch.h"
#include "Log.h"

namespace AstralEngine
{
	// Logger /////////////////////////////////////////////////////////////

	void Logger::Init(const char* filepath)
	{
		s_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		s_file = std::ofstream(filepath);
	}

	void Logger::Info(const std::string& prefix, const std::string format, ...)
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

	void Logger::Warn(const std::string& prefix, const std::string format, ...)
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

	void Logger::Error(const std::string& prefix, const std::string format, ...)
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

	std::string Logger::GetFormatedString(const std::string& format, va_list args)
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

				case 'l':
					ss << va_arg(args, long);
					break;

				case 'u':
					ss << va_arg(args, size_t);
					break;

				case 'L':
					ss << va_arg(args, unsigned long);
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

	void Logger::PrintInColor(const std::string& prefix, const std::string& message, WORD color)
	{
		SetConsoleTextAttribute(s_handle, color);
		s_file << prefix << message << "\n";
		std::cout << prefix << message << "\n";
		std::cout.flush();
		SetConsoleTextAttribute(s_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}