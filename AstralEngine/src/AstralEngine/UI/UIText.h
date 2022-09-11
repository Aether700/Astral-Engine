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
		TextCharacter(Texture2DHandle fontTextureAtlas, int topLeftCornerX, int topLeftCornerY, int width, 
			int height, int offsetX, int offsetY, int xAdvance);
		
		Vector2Int m_topLeftCornerCoords; //in pixels

		Vector2Int m_offset; //offset of the top left corner compared to the initial starting position of the virtual cursor
		int m_xAdvance; //How much to move the virtual cursor after rendering the current character
		Texture2DHandle m_fontTextureAtlas;
	};

	class Font
	{
	public:
		//returns a nullptr AReference if the file could not be open
		static AReference<Font> Create(const std::string& fontFilepath);

		const std::string& GetName() const { return m_name; }
		size_t GetLineHeight() const { return m_lineHeight; }
		const TextCharacter& GetCharacter(char c) const { return m_characters[c]; }
		const Texture2DHandle& GetFontAtlas() const { return m_fontAtlas; }

		bool HasCharacter(char c) const { return m_characters.ContainsKey(c); }

		int GetTopPadding() const { return m_topPadding; }
		int GetBottomPadding() const { return m_bottomPadding; }
		int GetLeftPadding() const { return m_leftPadding; }
		int GetRightPadding() const { return m_rightPadding; }

	private:
		std::string m_name;
		size_t m_lineHeight;
		AUnorderedMap<char, TextCharacter> m_characters;
		Texture2DHandle m_fontAtlas;
		int m_topPadding;
		int m_bottomPadding;
		int m_leftPadding;
		int m_rightPadding;
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
		const AReference<Font>& GetFont() const { return m_font; }
		const Vector4& GetColor() const { return m_color; }
		int GetPadding() const { return m_padding; }
		const AReference<Font>& GetFont() { return m_font; }
		
		void SetText(const std::string& text) { m_text = text; }
		void SetColor(const Vector4& color) { m_color = color; }
		void SetPadding(int padding) { m_padding = padding; }


	protected:
		void Draw() const override;

	private:
		//returns the new cursor position after rendering the word
		Vector2 RenderWord(std::string_view& word, Vector2 virtualCursorPos, float fontScale) const;

		//returns the position of the next whitespace in the string starting from the provided offset. 
		//if no white space has been found the position returned will be the end of the string
		size_t FindNextWhiteSpace(size_t offset) const;

		//moves the virtual cursor to skip the provided white space character
		Vector2 SkipWhiteSpace(char whiteSpace, const Vector2& cursorPos, float fontScale) const;

		size_t GetWordWidth(std::string_view& word, float fontScale) const;

		//Updates the virtual cursor position to change lines for the text
		Vector2 ChangeLines(const Vector2& cursorPos, float fontScale) const;

		bool ShouldChangeLines(const Vector2& cursorPos, float fontScale) const;

		std::string m_text;
		AReference<Font> m_font;
		Vector4 m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		int m_padding = 1;
	};
}