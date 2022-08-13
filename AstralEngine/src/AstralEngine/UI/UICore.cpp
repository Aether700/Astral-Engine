#include "aepch.h"
#include "UICore.h"
#include "Renderer/Renderer.h"

namespace AstralEngine
{
	//UIContext //////////////////////////////////////////////////////////////////////////////////////////


	//needs to be modified so the client doesn't have to add call this function from their code
	void UIContext::OnUpdate()
	{
		if (m_movingWindow)
		{
			MoveWindow();
		}
		else if (m_resizingWindow)
		{
			ResizeWindow();
		}
	}

	bool UIContext::OnEvent(AEvent& e) 
	{ 
		AEventDispatcher dispatcher(e);
		if (dispatcher.HandleAEvent<MouseButtonPressedEvent>(ADelegate<bool(MouseButtonPressedEvent&)>
			(function<&UIContext::OnMouseButtonPressed>, this)))
		{
			return true;
		}
		else if (dispatcher.HandleAEvent<MouseButtonReleasedEvent>(ADelegate<bool(MouseButtonReleasedEvent&)>
			(function<&UIContext::OnMouseButtonReleased>, this)))
		{
			return true;
		}
		
		for (AReference<UIWindow>& window : m_windows)
		{
			if (window->OnEvent(e))
			{
				return false;
			}
		}
		return false;
	}

	AReference<UIWindow> UIContext::CreateUIWindow(const Vector2& position, unsigned int width, unsigned int height)
	{
		AReference<UIWindow> newWindow = AReference<UIWindow>::Create(position, width, height);
		Application::GetUIContext()->m_windows.Add(newWindow);
		return newWindow;
	}

	AReference<UIWindow> UIContext::CreateUIWindow(const Vector2& position, unsigned int width, unsigned int height,
		UIWindowFlags flags, const Vector4& backgroundColor, Vector2Int minResize)
	{
		AReference<UIWindow> newWindow = AReference<UIWindow>::Create(position, width, height, flags, backgroundColor, minResize);
		Application::GetUIContext()->m_windows.Add(newWindow);
		return newWindow;
	}

	unsigned int UIContext::GetNumSpacesPerTab()
	{
		return Application::GetUIContext()->m_numSpacesPerTab;
	}

	void UIContext::SetNumSpacesPerTab(unsigned int numSpaces)
	{
		AE_CORE_ASSERT(numSpaces > 0, "Invalid number of spaces provided")
		Application::GetUIContext()->m_numSpacesPerTab = numSpaces;
	}

	void UIContext::RenderUI()
	{
		Renderer::BeginScene();
		for (AReference<UIWindow>& window : m_windows)
		{
			window->DrawToScreen();
		}
		Renderer::EndScene();
	}

	bool UIContext::OnMouseButtonPressed(MouseButtonPressedEvent& mousePressed)
	{
		if (mousePressed.GetButtonKeycode() == MouseButtonCode::Left)
		{
			if (m_focusedWindow != nullptr && m_focusedWindow->IsHovered())
			{
				ProcessUIWindowMouseButtonPress(mousePressed);
				return true;
			}

			for (AReference<UIWindow>& window : m_windows)
			{
				if (window->IsHovered())
				{
					m_focusedWindow = window->HasParent() ? window->GetParent() : window;
					ProcessUIWindowMouseButtonPress(mousePressed);
				}
			}
		}
		return false;
	}

	bool UIContext::OnMouseButtonReleased(MouseButtonReleasedEvent& mouseReleased)
	{
		if (mouseReleased.GetButtonKeycode() == MouseButtonCode::Left)
		{
			m_movingWindow = false;
			m_resizingWindow = false;
		}
		return false;
	}

	Vector2Short UIContext::CheckForResize(AReference<UIWindow>& window, Vector2 mousePos)
	{
		AE_CORE_ASSERT(window->IsHovered(), "window being checked for resize is not hovered");
		Vector2 displacement = mousePos - window->GetScreenCoords();
		unsigned int halfWidth = (unsigned int)window->GetScreenCoordsWidth() / 2;
		unsigned int halfHeight = (unsigned int)window->GetScreenCoordsHeight() / 2;

		short xComponent = Math::Abs(displacement.x) > (halfWidth - s_windowResizeBorder) ? 1 : 0;
		if (displacement.x < 0)
		{
			xComponent *= -1;
		}

		short yComponent = Math::Abs(displacement.y) > (halfHeight - s_windowResizeBorder) ? -1 : 0;
		if (displacement.y < 0)
		{
			yComponent *= -1;
		}
		return Vector2Short(xComponent, yComponent);
	}

	void UIContext::ProcessUIWindowMouseButtonPress(MouseButtonPressedEvent& pressed)
	{
		AReference<UIElement> hovered = m_focusedWindow->GetHoveredElement();

		if (hovered == nullptr)
		{
			m_lastMousePos = Input::GetMousePosition();
			m_resizeDir = CheckForResize(m_focusedWindow, m_lastMousePos);

			if (m_resizeDir == Vector2Short::Zero())
			{
				if (!(m_focusedWindow->GetFlags() & UIWindowFlagsNoMove))
				{
					m_movingWindow = true;
				}
			}
			else
			{
				if (!(m_focusedWindow->GetFlags() & UIWindowFlagsNoResize))
				{
					m_resizingWindow = true;
				}
			}
		}
		else
		{
			hovered->OnEvent(pressed);
		}
	}

	void UIContext::MoveWindow()
	{
		AE_CORE_ASSERT(m_focusedWindow != nullptr, "focused window has not been set");
		Vector2 currMousePos = Input::GetMousePosition();
		Vector2 displacement = currMousePos - m_lastMousePos;
		m_lastMousePos = currMousePos;

		m_focusedWindow->SetScreenCoords(m_focusedWindow->GetScreenCoords() + displacement);
	}

	void UIContext::ResizeWindow()
	{
		AE_CORE_ASSERT(m_focusedWindow != nullptr, "focused window has not been set");

		Vector2 currMousePos = Input::GetMousePosition();
		Vector2 displacement = currMousePos - m_lastMousePos;
		m_lastMousePos = currMousePos;

		if (m_resizeDir.x != 0)
		{
			int newWidth = ((int)m_focusedWindow->GetScreenCoordsWidth()) 
				+ (size_t)((float)m_resizeDir.x * displacement.x);

			if (newWidth < m_focusedWindow->GetMinResize().x)
			{
				//update displacement so it doesn't mess up the position of the window when resizing
				displacement.x = (m_focusedWindow->GetMinResize().x - 
					((int)m_focusedWindow->GetScreenCoordsWidth())) / m_resizeDir.x;
				newWidth = (int)m_focusedWindow->GetMinResize().x;
			}
			
			if (m_focusedWindow->GetScreenCoordsWidth() != m_focusedWindow->GetMinResize().x)
			{
				m_focusedWindow->SetScreenCoords(m_focusedWindow->GetScreenCoords() + Vector2(displacement.x / 2, 0));
			}

			m_focusedWindow->SetScreenCoordsWidth(newWidth);
		}

		if (m_resizeDir.y != 0)
		{
			int newHeight = ((int)m_focusedWindow->GetScreenCoordsHeight()) 
				+ (size_t)(- 1.0f * (float)m_resizeDir.y * displacement.y);
			if (newHeight < m_focusedWindow->GetMinResize().y)
			{
				//update displacement so it doesn't mess up the position of the window when resizing
				displacement.y = (m_focusedWindow->GetMinResize().y -
					((int)m_focusedWindow->GetScreenCoordsHeight())) / -m_resizeDir.y;
				newHeight = (int)m_focusedWindow->GetMinResize().y;
			}
			
			if (m_focusedWindow->GetScreenCoordsHeight() != m_focusedWindow->GetMinResize().y)
			{
				m_focusedWindow->SetScreenCoords(m_focusedWindow->GetScreenCoords() + Vector2(0, displacement.y / 2));
			}

			m_focusedWindow->SetScreenCoordsHeight(newHeight);
		}
	}

	//UIButton ///////////////////////////////////////////////
	UIButton* UIButton::s_clickedButton = nullptr;


}