#include "aepch.h"
#include "WindowsUtil.h"

namespace AstralEngine
{
	struct OpenGL1_1Module
	{
		static constexpr const char* s_moduleName = "opengl32.dll";
		HMODULE moduleHandle;
		OpenGL1_1Module()
		{
			moduleHandle = LoadLibrary(StrToWindowsStr(s_moduleName).c_str());

			if (moduleHandle == nullptr)
			{
				AE_CORE_ERROR("Could not load \'%s\'. Error Code: %L", s_moduleName, GetLastError());
			}
		}

		~OpenGL1_1Module()
		{
			if (FreeLibrary(moduleHandle) == 0)
			{
				AE_CORE_ERROR("Could not free \'%s\'. Error Code: %L", s_moduleName, GetLastError());
			}
		}
	};

	WindowsStr StrToWindowsStr(const std::string& str)
	{
		#ifdef UNICODE
			// fill wstring with space character then copy str to wstr
			std::wstring wstr = std::wstring(str.length(), ' ');
			std::copy(str.begin(), str.end(), wstr.begin());
			return wstr;
		#else
			return str;
		#endif
	}

	std::string WindowsStrToStr(const WindowsStr& windowStr)
	{
		#ifdef UNICODE
			// fill string with space character then copy wstr to str
			std::string str = std::string(windowStr.length(), ' ');
			std::copy(windowStr.begin(), windowStr.end(), str.begin());
			return str;
		#else
			return str;
		#endif
	}

	std::wstring WindowsStrToWStr(const WindowsStr& windowStr)
	{
		#ifdef UNICODE
			return windowStr;
		#else
			std::wstring wstr = std::wstring(windowStr.length, ' ');
			std::copy(windowStr.begin(), windowStr.end(), wstr.begin());
			return wstr;
		#endif
	}

	void* RetrieveOpenGLFunction(const char* functionName)
	{
		void* function = wglGetProcAddress(functionName);
		if (function == nullptr)
		{
			static OpenGL1_1Module opengl1_1Module;

			function = GetProcAddress(opengl1_1Module.moduleHandle, functionName);

			if (function == nullptr)
			{
				AE_CORE_ERROR("Could not load function \'%s\'. Error Code: %L", functionName, GetLastError());
			}
		}
		return function;
	}

	bool CheckIfOpenGLExtensionIsSupported(const char* extensionName)
	{
		// function which returns a list of the supported extensions
		static const char* (*getSupportedExtensionStr)()
			= (const char* (*)())wglGetProcAddress("wglGetExtensionsStringEXT");

		if (getSupportedExtensionStr == nullptr)
		{
			AE_CORE_ERROR("WindowsWindow could not retrieve \"wglGetExtensionsStringEXT\"");
			return false;
		}

		// check if the extention name is in the list
		return std::strstr(getSupportedExtensionStr(), extensionName) != nullptr;
	}
}