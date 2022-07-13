#include "aepch.h"
#include "WindowsUtil.h"

namespace AstralEngine
{
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
		// fill wstring with space character then copy str to wstr
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