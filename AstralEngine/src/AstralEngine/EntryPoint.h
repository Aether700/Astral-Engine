#pragma once

HANDLE AstralEngine::Logger::s_handle;
std::ofstream AstralEngine::Logger::s_file;

extern AstralEngine::Application* CreateApp();

int main()
{
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
}