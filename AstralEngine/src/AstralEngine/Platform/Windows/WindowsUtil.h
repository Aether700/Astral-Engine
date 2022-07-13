#pragma once
#include "AstralEngine/Data Struct/ADelegate.h"

namespace AstralEngine
{
	#ifdef UNICODE
		typedef std::wstring WindowsStr;
		typedef wchar_t WindowsChar;
	#else
		typedef std::string WindowsStr;
		typedef char WindowsChar;
	#endif

	WindowsStr StrToWindowsStr(const std::string& str);

	std::string WindowsStrToStr(const WindowsStr& windowStr);

	std::wstring WindowsStrToWStr(const WindowsStr& windowStr);

	bool CheckIfOpenGLExtensionIsSupported(const char* extensionName);

	template<typename Return, typename... Args>
	ADelegate<Return(Args...)> RetrieveOpenGLExtensionFunction(const char* functionName)
	{
		Return(*func)(Args...) = (Return(*)(Args...))wglGetProcAddress(functionName);
		if (func == nullptr)
		{
			AE_CORE_ERROR("Could not retrieve opengl function %s. Error Code: %L",
				functionName, GetLastError());
			return nullptr;
		}

		return ADelegate<Return(Args...)>(func);
	}
}