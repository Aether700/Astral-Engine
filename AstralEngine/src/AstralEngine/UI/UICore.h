#pragma once
#include "AstralEngine/Math/AMath.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Application.h"

#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Data Struct/ADynArr.h"

#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/AEvents/AEvent.h"
#include "AstralEngine/AEvents/MouseEvents.h"

namespace AstralEngine
{
	class UIWindow;

	enum UIWindowFlags
	{
		UIWindowFlagsNone = 0,
		UIWindowFlagsNoMove = 1 << 1,
		UIWindowFlagsNoResize = 1 << 2
	};

	/*manages all the window and widgets
	*/
	class UIContext : public Layer
	{
		friend class Scene;
		using WindowList = ADynArr<AReference<UIWindow>>;
	public:
		
		virtual void OnUpdate() override;

		virtual bool OnEvent(AEvent& e) override;

		static AReference<UIWindow> CreateUIWindow(const Vector2& position, unsigned int width, unsigned int height);

		static AReference<UIWindow> CreateUIWindow(const Vector2& position, unsigned int width, unsigned int height, 
			UIWindowFlags flags, const Vector4& backgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f }, 
			Vector2Int minResize = { 10, 10 });

		static unsigned int GetNumSpacesPerTab();
		static void SetNumSpacesPerTab(unsigned int numSpaces);

	private:

		void RenderUI();

		bool OnMouseButtonPressed(MouseButtonPressedEvent& mousePressed);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& mousePressed);

		/*returns values between -1 and 1 for both components, 0 meaning the axis is left 
		  unchanged �1 indicating the border selected for resize

		  up is defined as (0, 1) and (0, -1) indicates down
		*/
		Vector2Short CheckForResize(AReference<UIWindow>& window, Vector2 mousePos);

		/*updates the internal mechanism which selects if the window currently in focus is being moved 
		  or resized and in what direction

		  this function does NOT update which UIWindow is in focus 
		*/
		void ProcessUIWindowMouseButtonPress(MouseButtonPressedEvent& pressed);

		//moves/resizes the focused window
		void MoveWindow();
		void ResizeWindow();

		static constexpr unsigned int s_windowResizeBorder = 6; //in pixels

		WindowList m_windows;
		AReference<UIWindow> m_focusedWindow;
		bool m_movingWindow = false;
		bool m_resizingWindow = false;
		Vector2 m_lastMousePos;
		Vector2Short m_resizeDir;
		unsigned int m_numSpacesPerTab = 4;
	};

	/* Keeps track of the position, width and height of any UI element
	   as well as some helper functions such as IsHovered

	   The coordinates are in screen coordinates and the position represents 
	   the center of the UIElement
	*/
	class UIElement
	{
	public:
		UIElement(const Vector2& pos, size_t width, size_t height) 
			: m_pos(pos), m_width(width), m_height(height), m_minResize({10, 10}) { }

		UIElement(const Vector2& pos, size_t width, size_t height, Vector2 minResize)
			: m_pos(pos), m_width(width), m_height(height), m_minResize(minResize) { }

		UIElement(const UIElement& other) : m_pos(other.m_pos), m_width(other.m_width), m_height(other.m_height), 
			m_minResize(other.m_minResize), m_parentElement(other.m_parentElement) { }

		virtual ~UIElement() { }

		Vector2 GetScreenCoords() const 
		{ 
			if (m_parentElement != nullptr)
			{
				return m_parentElement->GetScreenCoords() + m_pos;
			}
			return m_pos; 
		}
		
		void SetScreenCoords(const Vector2& newPos) { m_pos = newPos; }

		Vector2 GetWorldPos() const 
		{
			return ScreenToWorldCoords(GetScreenCoords());
		}

		float GetWorldWidth() const 
		{
			//world width is from -1 to 1 hence the times 2
			return 2.0f * (float)m_width / (float)Application::GetWindow()->GetWidth();
		}

		float GetWorldHeight() const
		{
			//world height is from -1 to 1 hence the times 2
			return 2.0f * (float)m_height / (float)Application::GetWindow()->GetHeight();
		}

		size_t GetScreenCoordsWidth() const { return m_width; }
		size_t GetScreenCoordsHeight() const { return m_height; }

		//returns the smallests size possible when resizing as a Vector2. The size is in pixels
		const Vector2& GetMinResize() const { return m_minResize; }

		void SetScreenCoordsWidth(unsigned int w) { m_width = w; }
		void SetScreenCoordsHeight(unsigned int h) { m_height = h; }

		static Vector2 ScreenToWorldCoords(const Vector2& screenCoords)
		{
			unsigned int width = Application::GetWindow()->GetWidth();
			unsigned int height = Application::GetWindow()->GetHeight();

			//world width/height is from -1 to 1 hence the times 2
			return Vector2(2 * screenCoords.x / width - 1.0f, 1.0f - 2 * screenCoords.y / height);
		}

		bool IsHovered() const
		{
			Vector2 mousePos = Input::GetMousePosition();

			{
				const AWindow* window = Application::GetWindow();

				//if outside the window return false 
				if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > window->GetWidth()
					|| mousePos.y > window->GetHeight())
				{
					return false;
				}
			}

			//makes sure to get the proper screenCoords if parented
			Vector2 screenCoords = GetScreenCoords();

			Vector2 bottomLeftCorner = Vector2(screenCoords.x - m_width / 2, screenCoords.y + m_height / 2);
			Vector2 topRightCorner = Vector2(screenCoords.x + m_width / 2, screenCoords.y - m_height / 2);

			return mousePos.x >= bottomLeftCorner.x && mousePos.x <= topRightCorner.x
				&& mousePos.y <= bottomLeftCorner.y && mousePos.y >= topRightCorner.y;
		}

		AReference<UIElement>& GetParent() { return m_parentElement; }
		const AReference<UIElement>& GetParent() const { return m_parentElement; }
		void SetParent(AReference<UIElement> newParent) { m_parentElement = newParent; }
		bool HasParent() const { return m_parentElement != nullptr; }

		virtual bool OnEvent(AEvent& e) { return false; }

		virtual bool operator==(const UIElement& other) const
		{
			return m_pos == other.m_pos && m_width == other.m_width
				&& m_height == other.m_height && m_minResize == other.m_minResize
				&& m_parentElement == other.m_parentElement;
		}

		bool operator!=(const UIElement& other) const
		{
			return !(*this == other);
		}

	private:
		Vector2 m_pos;
		size_t m_width, m_height;
		AReference<UIElement> m_parentElement;
		Vector2 m_minResize; //the smallest size allowed when resizing in pixels
	};

	class RenderableUIElement : public UIElement
	{
	public:
		RenderableUIElement(const Vector2& pos, size_t width, size_t height)
			: UIElement(pos, width, height) { }

		RenderableUIElement(const Vector2& pos, size_t width, size_t height, Vector2 minResize)
			: UIElement(pos, width, height, minResize) { }

		void DrawToScreen() const
		{
			if (m_renderFunc.HasFunction())
			{
				m_renderFunc(this);
			}
			else
			{
				Draw();
			}
		}

		void SetRenderingFunction(ADelegate<void(const UIElement*)> func) { m_renderFunc = func; }

	protected:
		virtual void Draw() const = 0;
	private:
		ADelegate<void(const UIElement*)> m_renderFunc = nullptr;
	};

	//basic UI window which will contain all the widgets
	class UIWindow : public RenderableUIElement
	{
	public:
		UIWindow(const Vector2& position, unsigned int width, unsigned int height)
			: RenderableUIElement(position, width, height), m_backgroundColor({ 0.1f, 0.1f, 0.1f, 1.0f }),
			m_flags(UIWindowFlagsNone) { }

		UIWindow(const Vector2& position, unsigned int width, unsigned int height, UIWindowFlags flags,
			const Vector4& backgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f }, Vector2Int minResize = { 10, 10 })
			: RenderableUIElement(position, width, height, minResize), m_backgroundColor(backgroundColor), m_flags(flags) { }

		const Vector4& GetBackgroundColor() const { return m_backgroundColor; }

		UIWindowFlags GetFlags() const { return m_flags; }

		void SetFlags(const UIWindowFlags flags) { m_flags = flags; }


		void AddElement(AReference<RenderableUIElement>& element)
		{
			m_elements.Add(element);
		}

		void RemoveElement(AReference<RenderableUIElement>& element)
		{
			m_elements.Remove(element);
		}

		//returns the first hovered element in the window or nullptr if none is hovered
		AReference<UIElement> GetHoveredElement()
		{
			for (AReference<RenderableUIElement>& element : m_elements)
			{
				if (element->IsHovered())
				{
					return element;
				}
			}
			return nullptr;
		}


		virtual bool OnEvent(AEvent& e) override
		{
			for (AReference<RenderableUIElement>& element : m_elements)
			{
				if (element->OnEvent(e))
				{
					return true;
				}
			}
			return false;
		}

		bool operator==(const UIElement& other) const override
		{
			const UIWindow* otherWindow = dynamic_cast<const UIWindow*>(&other);
			if (otherWindow == nullptr)
			{
				return false;
			}
			return UIElement::operator==(other) && m_backgroundColor == otherWindow->m_backgroundColor 
				&& m_flags == otherWindow->m_flags;
		}

	protected:
		void Draw() const override
		{ 
			for (const AReference<RenderableUIElement>& element : m_elements)
			{
				element->DrawToScreen();
			}
			Renderer::DrawUIElement(*this, m_backgroundColor);
		}

	private:
		ASinglyLinkedList<AReference<RenderableUIElement>> m_elements; //can include windows
		Vector4 m_backgroundColor;
		UIWindowFlags m_flags;
	};

	class UIButton : public RenderableUIElement
	{
	public:
		UIButton(const std::string& text, const Vector4& color) 
			: RenderableUIElement({ 0, 0 }, 75, 75), m_text(text), m_defaultColor(color),
			m_hoveredColor(1.2f * color), m_buttonPressedColor(0.8f * color) { }

		UIButton(const Vector2& pos, size_t width, size_t height, const std::string& text) 
			: RenderableUIElement(pos, width, height), m_text(text), m_defaultColor({0.5, 0.5, 0.5, 1}),
			m_hoveredColor(1.2f * m_defaultColor), m_buttonPressedColor(0.8f * m_defaultColor) { }

		const Vector4& GetDefaultColor() const { return m_defaultColor; }
		const Vector4& GetHoveredColor() const { return m_hoveredColor; }
		const Vector4& GetButtonPressedColor() const { return m_buttonPressedColor; }

		void SetDefaultColor(const Vector4& color) { m_defaultColor = color; }
		void SetHoveredColor(const Vector4& color) { m_hoveredColor = color; }
		void SetButtonPressedColor(const Vector4& color) { m_buttonPressedColor = color; }

		void AddListener(const ADelegate<void()>& listener)
		{
			m_listeners.AddDelegate(listener);
		}

		void RemoveListener(const ADelegate<void()>& listener)
		{
			m_listeners.RemoveDelegate(listener);
		}

		virtual bool OnEvent(AEvent& e)
		{
			AEventDispatcher dispatcher(e);

			if (dispatcher.HandleAEvent<MouseButtonPressedEvent>
				(ADelegate<bool(MouseButtonPressedEvent&)>(function<&UIButton::OnMouseButtonPressed>, this)))
			{
				return true;
			}

			if (dispatcher.HandleAEvent<MouseButtonReleasedEvent>
				(ADelegate<bool(MouseButtonReleasedEvent&)>(function<&UIButton::OnMouseButtonReleased>, this)))
			{
				return true;
			}

			return false;
		}

	protected:
		virtual void Draw() const override
		{
			Vector4 colorToDraw = m_defaultColor;
			if (s_clickedButton == this)
			{
				colorToDraw = m_buttonPressedColor;
			}
			else if (IsHovered())
			{
				colorToDraw = m_hoveredColor;
			}
		
			Renderer::DrawUIElement(*this, colorToDraw);
		}
	private:
		
		bool OnMouseButtonPressed(MouseButtonPressedEvent& pressed)
		{
			if (pressed.GetButtonKeycode() == AE_MOUSE_BUTTON_LEFT)
			{
				if (IsHovered())
				{
					s_clickedButton = this;
					return true;
				}
			}
			return false;
		}

		bool OnMouseButtonReleased(MouseButtonReleasedEvent& pressed)
		{
			if (pressed.GetButtonKeycode() == AE_MOUSE_BUTTON_LEFT)
			{
				if (s_clickedButton == this && IsHovered())
				{
					s_clickedButton = nullptr;
					ClickButton();
					return true;
				}
			}
			s_clickedButton = nullptr;
			return false;
		}

		//gets called when the button is clicked
		void ClickButton()
		{
			m_listeners.CallDelagates();
		}

		static UIButton* s_clickedButton; //keeps track of the last clicked button

		std::string m_text;

		Vector4 m_defaultColor;
		Vector4 m_hoveredColor;
		Vector4 m_buttonPressedColor;
		
		SignalHandler<void()> m_listeners;
	};
}