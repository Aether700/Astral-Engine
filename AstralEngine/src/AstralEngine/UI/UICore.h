#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Application.h"

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

		const Vector2& GetScreenCoords() const { return m_pos; }
		
		Vector2 GetWorldPos() const 
		{
			unsigned int width = Application::GetApp()->GetWindow()->GetWidth();
			unsigned int height = Application::GetApp()->GetWindow()->GetHeight();
			return Vector2(m_pos.x / width - 1.0f, 1.0f - m_pos.y / height );
		}

		float GetWorldWidth() const 
		{
			return (float)m_width / (float)Application::GetApp()->GetWindow()->GetWidth();
		}

		float GetWorldHeight() const
		{
			return (float)m_height / (float)Application::GetApp()->GetWindow()->GetHeight();
		}

		size_t GetScreenCoordsWidth() const { return m_width; }
		size_t GetScreenCoordsHeight() const { return m_height; }

		bool IsHovered() const
		{
			Vector2 mousePos = Input::GetMousePosition();

			Vector2 bottomLeftCorner = Vector2(m_pos.x - m_width / 2, m_pos.y + m_height / 2);
			Vector2 topRightCorner = Vector2(m_pos.x + m_width / 2, m_pos.y - m_height / 2);

			return mousePos.x >= bottomLeftCorner.x && mousePos.x <= topRightCorner.x
				&& mousePos.y <= bottomLeftCorner.y && mousePos.y >= topRightCorner.y;
		}

	private:
		Vector2 m_pos;
		size_t m_width, m_height;
	};
}