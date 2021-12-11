#include "aepch.h"
#include "Application.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "Core.h"
#include "Time.h"
#include "AstralEngine/UI/UICore.h"

#include <glad/glad.h>

namespace AstralEngine
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& windowTitle, unsigned int width, unsigned int height) 
		: m_isRunning(true), m_minimized(false)
	{
		AE_PROFILE_FUNCTION();
		AE_CORE_ASSERT((s_instance == nullptr), "Creating Duplicate Application Instance.");

		s_instance = this;
		m_window = AWindow::Create(windowTitle, width, height);
		m_uiContext = new UIContext();

		m_layerStack.AttachLayer(m_uiContext);

		Renderer::Init();
		Random::Init();

		m_window->SetEventCallback(ADelegate<void(AEvent&)>(FunctionWraper<&Application::OnEvent>(), this));
	}

	Application::~Application() 
	{
		//no need to delete the UIContext since the LayerStack will do it for us
		AE_PROFILE_FUNCTION();
		Renderer::Shutdown();
		delete m_window;
	}

	void Application::OnEvent(AEvent& e)
	{
		AE_PROFILE_FUNCTION();
		AEventDispatcher dispatcher(e);

		dispatcher.HandleAEvent<WindowCloseEvent>(ADelegate<bool(WindowCloseEvent&)>
			(function<&Application::OnWindowCloseEvent>, this));
		dispatcher.HandleAEvent<WindowResizeEvent>(ADelegate<bool(WindowResizeEvent&)>
			(function<&Application::OnWindowResizeEvent>, this));

		for (int i = 0; i < m_layerStack.GetCount(); i++)
		{
			if (m_layerStack[i]->OnEvent(e))
			{
				break;
			}
		}
	}

	void Application::AttachLayer(Layer* l) 
	{
		AE_PROFILE_FUNCTION();
		GetApp()->m_layerStack.AttachLayer(l); 
	}
	
	void Application::DetachLayer(Layer* l) 
	{
		AE_PROFILE_FUNCTION();
		GetApp()->m_layerStack.DetachLayer(l);
	}
	
	void Application::AttachOverlay(Layer* l) 
	{
		AE_PROFILE_FUNCTION();
		GetApp()->m_layerStack.AttachOverlay(l);
	}
	
	void Application::DetachOverlay(Layer* l) 
	{
		AE_PROFILE_FUNCTION();
		GetApp()->m_layerStack.DetachLayer(l);
	}

	void Application::Run()
	{
		AE_PROFILE_FUNCTION();
		while(m_isRunning)
		{
			AE_PROFILE_SCOPE("Run loop");

			if (!m_minimized)
			{
				AE_PROFILE_SCOPE("Updating layers (OnUpdate)");
				Time::UpdateTime();
				for (int i = 0; i < m_layerStack.GetCount(); i++)
				{
					m_layerStack[i]->OnUpdate();
				}
			}

			m_window->OnUpdate();
		}
	}

	Application* Application::GetApp()
	{
		return s_instance;
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& close)
	{
		m_isRunning = false;
		return true;
	}

	bool Application::OnWindowResizeEvent(WindowResizeEvent& resize) 
	{
		if (resize.GetWidth() == 0 || resize.GetHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		RenderCommand::SetViewport(0, 0, resize.GetWidth(), resize.GetHeight());
		return false;
	}

}