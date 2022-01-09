#pragma once
#include "AstralEngine/AEvents/AEvent.h"
#include "AstralEngine/AEvents/AppEvents.h"
#include "LayerStack.h"
#include "AWindow.h"

namespace AstralEngine
{
	class UIContext;
	class PhysicsEngine2D;

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
		static PhysicsEngine2D* GetPhysicsEngine2D() { return GetApp()->m_physics2D; }

		static void Exit() { GetApp()->m_isRunning = false; }

	private:
		static Application* GetApp();

		bool OnWindowCloseEvent(WindowCloseEvent& close);
		bool OnWindowResizeEvent(WindowResizeEvent& resize);

		bool m_isRunning;
		LayerStack m_layerStack;
		AWindow* m_window;
		UIContext* m_uiContext;
		PhysicsEngine2D* m_physics2D;
		bool m_minimized;

		static Application* s_instance;
	};

	Application* CreateApp();
}