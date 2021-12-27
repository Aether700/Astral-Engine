#pragma once
#include "UICore.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"
#include "AstralEngine/Data Struct/AReference.h"
#include <string>
#include <array>

namespace AstralEngine
{
	class TextCharacter : public UIElement
	{
		friend class Font;
	public:
		TextCharacter();
		TextCharacter(const TextCharacter& other);

		const Vector2Int& GetTopLeftCornerCoords() const { return m_topLeftCornerCoords; }
		const Vector2Int& GetOffset() const { return m_offset; }
		int GetXAdvance() const { return m_xAdvance; }
		const std::array<Vector2, 4> GetTextureCoords() const;

	private:
		TextCharacter(AReference<Texture2D>& fontTextureAtlas, int topLeftCornerX, int topLeftCornerY, int width, 
			int height, int offsetX, int offsetY, int xAdvance);
		
		Vector2Int m_topLeftCornerCoords; //in pixels

		Vector2Int m_offset; //offset of the top left corner compared to the initial starting position of the virtual cursor
		int m_xAdvance; //How much to move the virtual cursor after rendering the current character
		AReference<Texture2D> m_fontTextureAtlas;
	};

	class Font
	{
	public:
		//returns a nullptr AReference if the file could not be open
		static AReference<Font> Create(const std::string& fontFilepath);

		const std::string& GetName() const { return m_name; }
		size_t GetLineHeight() const { return m_lineHeight; }
		const TextCharacter& GetCharacter(char c) const { return m_characters[c]; }
		const AReference<Texture2D>& GetFontAtlas() const { return m_fontAtlas; }
	private:
		std::string m_name;
		size_t m_lineHeight;
		AUnorderedMap<char, TextCharacter> m_characters;
		AReference<Texture2D> m_fontAtlas;
	};

	class TextElement : public RenderableUIElement
	{
	public:
		TextElement();
		TextElement(const std::string& text);
		TextElement(AReference<Font>& font);
		TextElement(AReference<Font>& font, const std::string& text);
		TextElement(const Vector2& pos, size_t width, size_t height);
		TextElement(const Vector2& pos, size_t width, size_t height, const std::string& text);
		TextElement(const Vector2& pos, size_t width, size_t height, AReference<Font>& font, const std::string& text);

		const std::string& GetText() const { return m_text; }
		void SetText(const std::string& text) { m_text = text; }
		const AReference<Font>& GetFont() const { return m_font; }
		const AReference<Font>& GetFont() { return m_font; }
		const Vector4& GetColor() const { return m_color; }
		void SetColor(const Vector4& color) { m_color = color; }

	protected:
		void Draw() const override;

	private:
		std::string m_text;
		AReference<Font> m_font;
		Vector4 m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}