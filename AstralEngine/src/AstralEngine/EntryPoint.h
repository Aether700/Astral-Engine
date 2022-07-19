#pragma once

HANDLE AstralEngine::Logger::s_handle;
std::ofstream AstralEngine::Logger::s_file;

extern AstralEngine::Application* CreateApp();

int main()
{

	AstralEngine::Logger::Init();
	AstralEngine::Application* app = CreateApp();
	std::ifstream file = std::ifstream("assets/fonts/arial.ttf", std::ios_base::binary | std::ios_base::ate);

	size_t fileSize = file.tellg();
	file.seekg(0);
	std::uint8_t* data = new std::uint8_t[fileSize + 1];
	file.read((char*)&data[0], fileSize);
	std::uint8_t* dataFlipped = new std::uint8_t[fileSize + 1];
	AstralEngine::AssertDataEndianness(data, dataFlipped, fileSize, AstralEngine::Endianness::BigEndian);
	data[fileSize] = '\0';
	dataFlipped[fileSize] = '\0';
	
	std::cout << "data:\n" << (char*)data << "\n\n";
	std::cout << "dataFlipped:\n" << (char*)dataFlipped << "\n\n";

	delete data;
	delete dataFlipped;
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