#pragma once
#include "AstralEngine/AEvents/AEvent.h"
#include "AstralEngine/AEvents/AppEvents.h"
#include "LayerStack.h"
#include "AWindow.h"

namespace AstralEngine
{
	class Application
	{
	public:
		Application(const std::string& windowTitle = "Astral Engine", unsigned int width = 1280, unsigned int height = 760);
		virtual ~Application();

		void Run();
		void OnEvent(AEvent& e);

		void AttachLayer(Layer* l);
		void DetachLayer(Layer* l);
		void AttachOverlay(Layer* l);
		void DetachOverlay(Layer* l);

		inline AWindow* GetWindow() const { return m_window; }

		static const Application* GetApp();

	private:

		bool OnWindowCloseEvent(WindowCloseEvent& close);
		bool OnWindowResizeEvent(WindowResizeEvent& resize);

		bool m_isRunning;
		LayerStack m_layerStack;
		AWindow* m_window;
		bool m_minimized;

		static Application* s_instance;
	};

	Application* CreateApp();
}