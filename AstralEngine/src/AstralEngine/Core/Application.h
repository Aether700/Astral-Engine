#pragma once
#include "AstralEngine/AEvents/AEvent.h"
#include "AstralEngine/AEvents/AppEvents.h"
#include "LayerStack.h"
#include "AWindow.h"

namespace AstralEngine
{
	class UIContext;

	enum class Endianness
	{
		LittleEndian, // for 10 store -> 01
		BigEndian // for 10 store -> 10
	};

	class Application
	{
	public:
		Application(const std::string& windowTitle = "Astral Engine", unsigned int width = 1280, unsigned int height = 760);
		virtual ~Application();

		void Run();
		void OnEvent(AEvent& e);

		static void AttachLayer(Layer* l);
		static void DetachLayer(Layer* l);
		static void AttachOverlay(Layer* l);
		static void DetachOverlay(Layer* l);

		static AWindow* GetWindow() { return GetApp()->m_window; }
		static UIContext* GetUIContext() { return GetApp()->m_uiContext; }

		static void Exit() { GetApp()->m_isRunning = false; }

		static Endianness SystemEndianness();

	private:
		static Application* GetApp();

		bool OnWindowCloseEvent(WindowCloseEvent& close);
		bool OnWindowResizeEvent(WindowResizeEvent& resize);

		void RetrieveSystemEndianess();

		bool m_isRunning;
		LayerStack m_layerStack;
		AWindow* m_window;
		UIContext* m_uiContext;
		bool m_minimized;

		Endianness m_systemEndianness;

		static Application* s_instance;
	};

	// copies the data provided into the destination provided in the system endianness assuming the data 
	// is in the provided endianness. The dataSize is in bytes
	void AssertDataEndianness(void* data, void* dest, size_t dataSize, Endianness endiannessBeingRead);

}

AstralEngine::Application* CreateApp();
