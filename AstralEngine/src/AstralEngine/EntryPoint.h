#pragma once

HANDLE AstralEngine::Logger::s_handle;
std::ofstream AstralEngine::Logger::s_file;

extern AstralEngine::Application* CreateApp();

int main()
{
	using namespace AstralEngine;

	Transform t = Transform({ 0, 0, 0 }, { 0, 90, 0 }, {1, 1, 1});
	Mat4 m = t.GetTransformMatrix();

	Quaternion q = Quaternion::FromEuler(0, 90, 0);
	Mat4 m2 = q.GetRotationMatrix();
	bool equal = m == m2;


	

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