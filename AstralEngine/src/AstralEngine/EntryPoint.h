#pragma once

HANDLE AstralEngine::Logger::s_handle;
std::ofstream AstralEngine::Logger::s_file;

extern AstralEngine::Application* CreateApp();

// temp //////////////////////////////////////////////////
std::wstring StrToWStr(const std::string& str)
{
	std::wstring wstr = std::wstring(str.length(), ' ');// fill string with spaceCharacter
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

#include "Platform/Windows/WindowsWindow.h"
using namespace AstralEngine;
///////////////////////////////////////////////

int main()
{
	/*
#ifdef UNICODE
	const auto className = StrToWStr("WindowsWindow");
#else
	const auto className = "WindowsWindow";
#endif

	HINSTANCE hInstance = GetModuleHandle(nullptr);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className.c_str();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(
		0, className.c_str(),
		StrToWStr("AstralEngine").c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480,
		nullptr, nullptr, hInstance, nullptr
	);
	// show the damn window
	ShowWindow(hWnd, SW_SHOW);
	*/
	WindowsWindow w = WindowsWindow("AstralEngine", 200, 200, 640, 480);
	while (true)
	{
		w.OnUpdate();
	}

	/*
	AE_PROFILE_BEGIN_SESSION("Startup", "AstralEngine-Startup.json");
	AstralEngine::Logger::Init();
	AstralEngine::Application* app = CreateApp();
	AE_PROFILE_END_SESSION();

	AE_PROFILE_BEGIN_SESSION("Runtime", "AstralEngine-Runtime.json");
	app->Run();
	AE_PROFILE_END_SESSION();

	AE_PROFILE_BEGIN_SESSION("Shutdown", "AstralEngine-Shutdown.json");
	delete app;
	AE_PROFILE_END_SESSION();
	*/
}

#ifndef AE_DEBUG
	#ifdef AE_PLATFORM_WINDOWS
		int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
		{
			main();
		}
	#endif
#endif