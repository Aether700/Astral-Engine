#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Application.h"

#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	/* Keeps track of the position, width and height of any UI element
	   as well as some helper functions such as IsHovered

	   The coordinates are in screen coordinates and the position represents the center of the UIElement
	*/
	class UIElement
	{
	public:
		UIElement(const Vector2& pos, size_t width, size_t height) 
			: m_pos(pos), m_width(width), m_height(height) 
		{
			AE_CORE_INFO("width: %d\nheigh: %d", m_width, m_height);
		}

		const Vector2& GetScreenCoords() const 
		{ 
			if (m_parentElement != nullptr)
			{
				return m_parentElement->GetScreenCoords() + m_pos;
			}
			return m_pos; 
		}
		
		Vector2 GetWorldPos() const 
		{
			unsigned int width = Application::GetApp()->GetWindow()->GetWidth();
			unsigned int height = Application::GetApp()->GetWindow()->GetHeight();

			Vector2 screenCoords = GetScreenCoords();

			//world width/height is from -1 to 1 hence the times 2
			return Vector2(2 * screenCoords.x / width - 1.0f, 1.0f - 2 * screenCoords.y / height );
		}

		float GetWorldWidth() const 
		{
			//world width is from -1 to 1 hence the times 2
			return 2.0f * (float)m_width / (float)Application::GetApp()->GetWindow()->GetWidth();
		}

		//why the 2?
		float GetWorldHeight() const
		{
			//world height is from -1 to 1 hence the times 2
			return 2.0f * (float)m_height / (float)Application::GetApp()->GetWindow()->GetHeight();
		}

		size_t GetScreenCoordsWidth() const { return m_width; }
		size_t GetScreenCoordsHeight() const { return m_height; }

		bool IsHovered() const
		{
			Vector2 mousePos = Input::GetMousePosition();

			const Application* app = Application::GetApp();

			//if outside the window return false 
			if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > app->GetWindow()->GetWidth() 
				|| mousePos.y > app->GetWindow()->GetHeight())
			{
				return false;
			}

			//makes sure to get the proper screenCoords if parented
			Vector2 screenCoords = GetScreenCoords();

			Vector2 bottomLeftCorner = Vector2(screenCoords.x - m_width / 2, screenCoords.y + m_height / 2);
			Vector2 topRightCorner = Vector2(screenCoords.x + m_width / 2, screenCoords.y - m_height / 2);

			return mousePos.x >= bottomLeftCorner.x && mousePos.x <= topRightCorner.x
				&& mousePos.y <= bottomLeftCorner.y && mousePos.y >= topRightCorner.y;
		}

		void SetParent(AReference<UIElement> newParent) { m_parentElement = newParent; }

	private:
		Vector2 m_pos;
		size_t m_width, m_height;
		AReference<UIElement> m_parentElement;
	};
}