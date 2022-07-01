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
		RetrieveSystemEndianess();
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

			Input::OnUpdate();
			m_window->OnUpdate();
		}
	}

	void Application::OnEvent(AEvent& e)
	{
		AE_PROFILE_FUNCTION();
		AEventDispatcher dispatcher(e);

		dispatcher.HandleAEvent<WindowCloseEvent>(ADelegate<bool(WindowCloseEvent&)>
			(function<&Application::OnWindowCloseEvent>, this));
		dispatcher.HandleAEvent<WindowResizeEvent>(ADelegate<bool(WindowResizeEvent&)>
			(function<&Application::OnWindowResizeEvent>, this));
		dispatcher.HandleAEvent<KeyPressedEvent>(ADelegate<bool(KeyPressedEvent&)>
			(&Input::OnKeyPressedEvent));
		dispatcher.HandleAEvent<KeyReleasedEvent>(ADelegate<bool(KeyReleasedEvent&)>
			(&Input::OnKeyReleasedEvent));
		dispatcher.HandleAEvent<MouseButtonPressedEvent>(ADelegate<bool(MouseButtonPressedEvent&)>
			(&Input::OnMousePressedEvent));
		dispatcher.HandleAEvent<MouseButtonReleasedEvent>(ADelegate<bool(MouseButtonReleasedEvent&)>
			(&Input::OnMouseReleasedEvent));

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

	Endianness Application::SystemEndianness()
	{
		return GetApp()->m_systemEndianness;
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

	void Application::RetrieveSystemEndianess()
	{
		std::uint16_t data = 1;
		std::uint8_t* byte = (std::uint8_t*) &data;
		if ( *byte == 0 )
		{
			m_systemEndianness = Endianness::BigEndian;
		}
		else
		{
			m_systemEndianness = Endianness::LittleEndian;
		}
	}



	void AssertDataEndianness(void* data, void* dest, size_t dataSize, Endianness endiannessBeingRead)
	{
		if (Application::SystemEndianness() == endiannessBeingRead)
		{
			memcpy(dest, data, dataSize);
			return;
		}

		std::uint8_t* dataBytes = reinterpret_cast<std::uint8_t*>(data);
		std::uint8_t* destBytes = reinterpret_cast<std::uint8_t*>(dest);

		for (size_t i = 0; i < dataSize; i++)
		{
			destBytes[i] = dataBytes[dataSize - 1 - i];
		}
	}
}