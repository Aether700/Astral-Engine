#include "aepch.h"
#include "UIText.h"
#include <fstream>
#include <sstream>


namespace AstralEngine
{
	//TextCharacter///////////////////////////////////
	TextCharacter::TextCharacter() : UIElement(Vector2::Zero(), 0, 0) { }
	TextCharacter::TextCharacter(const TextCharacter& other) : UIElement(other), 
		m_fontTextureAtlas(other.m_fontTextureAtlas), m_topLeftCornerCoords(other.m_topLeftCornerCoords), 
		m_offset(other.m_offset), m_xAdvance(other.m_xAdvance) { }
	TextCharacter::TextCharacter(Texture2DHandle fontTextureAtlas, int topLeftCornerX, int topLeftCornerY, int width,
		int height, int offsetX, int offsetY, int xAdvance) : UIElement(Vector2(topLeftCornerX + width * 0.5f, 
			topLeftCornerY + height * 0.5f), width, height), m_fontTextureAtlas(fontTextureAtlas), 
		m_topLeftCornerCoords(Vector2Int(topLeftCornerX, topLeftCornerY)), m_offset(Vector2Int(offsetX, offsetY)), 
		m_xAdvance(xAdvance) { }

	const std::array<Vector2, 4> TextCharacter::GetTextureCoords() const
	{
		AReference<Texture2D> fontTextureAtlas = ResourceHandler::GetTexture2D(m_fontTextureAtlas);
		const std::array<Vector2, 4> textureCoords = {
			Vector2((float)m_topLeftCornerCoords.x / (float)fontTextureAtlas->GetWidth(),
				((float)(fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y 
				- GetScreenCoordsHeight())) / (float)fontTextureAtlas->GetHeight()),
			Vector2(((float)(m_topLeftCornerCoords.x + GetScreenCoordsWidth())) / (float)fontTextureAtlas->GetWidth(),
				((float)(fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y
					- GetScreenCoordsHeight())) / (float)fontTextureAtlas->GetHeight()),
			Vector2(((float)(m_topLeftCornerCoords.x + GetScreenCoordsWidth())) / (float)fontTextureAtlas->GetWidth(),
				(float)(fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y)
				/ (float)fontTextureAtlas->GetHeight()),
			Vector2((float)m_topLeftCornerCoords.x / (float)fontTextureAtlas->GetWidth(),
				(float)(fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y)
				/ (float)fontTextureAtlas->GetHeight())
		};
		return textureCoords;
	}

	//Font/////////////////////////////////////////////

	//Struct which stores the data of a character entry in the font file 
	struct CharacterData
	{
		int id;
		int x;
		int y;
		int width;
		int height;
		int offsetX;
		int offsetY;
		int xAdvance;
		int page;
		int channel;

		bool operator==(const CharacterData& other) const
		{
			return id == other.id;
		}

		bool operator!=(const CharacterData& other) const
		{
			return !(*this == other);
		}

	};

	struct HeaderInfo
	{
		std::string fontName;
		std::string fontAtlasFileName;
		int lineHeight;
		int charCount;
		int padding[4];
	};

	static std::string ReadLine(std::ifstream& file)
	{
		std::stringstream ss;
		while (file.peek() != EOF)
		{
			char currChar = file.get();
			if (currChar == '\n')
			{
				break;
			}
			ss << currChar;
		}
		return ss.str();
	}

	static HeaderInfo ProcessHeader(std::ifstream& file)
	{
		HeaderInfo info;
		bool inHeader = true;

		while (inHeader && file.peek() != EOF)
		{
			std::string line = ReadLine(file);
			size_t currPos = 0;
			while (currPos < line.length())
			{
				size_t equalsPos = line.find("=", currPos);
				size_t startField = line.find_first_not_of(" ", currPos);
				std::string fieldName = line.substr(startField, equalsPos - startField);
				size_t endFieldVal = line.find(" ", equalsPos);
				std::string fieldVal = line.substr(equalsPos + 1, endFieldVal - (equalsPos + 1));

				if (fieldName == "info face")
				{
					info.fontName = fieldVal.substr(1, fieldVal.length() - 2);
				}
				else if (fieldName == "file")
				{
					info.fontAtlasFileName = fieldVal.substr(1, fieldVal.length() - 2);
				}
				else if (fieldName == "common lineHeight")
				{
					info.lineHeight = atoi(fieldVal.c_str());
				}
				else if (fieldName == "chars count")
				{
					info.charCount = atoi(fieldVal.c_str());
					inHeader = false;
					break;
				}
				else if(fieldName == "padding")
				{
					size_t currPos = 0;
					int i = 0;
					while (currPos < fieldVal.length() && i < sizeof(info.padding) / sizeof(int)) 
					{
						size_t last = currPos;
						currPos = fieldVal.find(",", currPos);
						info.padding[i] = atoi(fieldVal.substr(last, currPos - last).c_str());
						i++;
						currPos++; //skip over ','
					}
				}

				currPos = endFieldVal + 1;
				if (endFieldVal == std::string::npos)
				{
					break;
				}
			}
		}

		return info;
	}

	static CharacterData ProcessChar(const std::string& line)
	{
		CharacterData data;
		size_t currPos = 0;

		while (currPos < line.length())
		{
			size_t equalsPos = line.find("=", currPos);
			size_t startField = line.find_first_not_of(" ", currPos);
			std::string fieldName = line.substr(startField, equalsPos - startField);
			size_t endFieldVal = line.find(" ", equalsPos);
			int fieldVal = atoi(line.substr(equalsPos + 1, endFieldVal - (equalsPos + 1)).c_str());

			if (fieldName == "char id")
			{
				data.id = fieldVal;
			}
			else if (fieldName == "x")
			{
				data.x = fieldVal;
			}
			else if (fieldName == "y")
			{
				data.y = fieldVal;
			}
			else if (fieldName == "width")
			{
				data.width = fieldVal;
			}
			else if (fieldName == "height")
			{
				data.height = fieldVal;
			}
			else if (fieldName == "xoffset")
			{
				data.offsetX = fieldVal;
			}
			else if (fieldName == "yoffset")
			{
				data.offsetY = fieldVal;
			}
			else if (fieldName == "xadvance")
			{
				data.xAdvance = fieldVal;
			}
			else if (fieldName == "page")
			{
				data.page = fieldVal;
			}
			else if (fieldName == "chnl")
			{
				data.channel = fieldVal;
			}
			else
			{
				AE_CORE_ERROR("Unknown field '%S' found", fieldName);
			}

			currPos = endFieldVal + 1;
		}

		return data;
	}

	static Texture2DHandle RetrieveFontTextureAtlas(const std::string& fontFilepath, 
		const std::string& fontAtlasFileName)
	{
		size_t lastSlash = fontFilepath.find_last_of("/");

		if (lastSlash == std::string::npos)
		{
			return ResourceHandler::LoadTexture2D(fontAtlasFileName);
		}

		std::stringstream ss;
		ss << fontFilepath.substr(0, lastSlash + 1);
		ss << fontAtlasFileName;

		return ResourceHandler::LoadTexture2D(ss.str());
	}

	AReference<Font> Font::Create(const std::string& fontFilepath)
	{
		std::ifstream fontFile = std::ifstream(fontFilepath);

		if (!fontFile)
		{
			AE_CORE_ERROR("Could not open file '%S'", fontFilepath);
			return nullptr;
		}

		AReference<Font> font = AReference<Font>::Create();

		HeaderInfo header = ProcessHeader(fontFile);
		font->m_name = header.fontName;
		font->m_lineHeight = header.lineHeight;
		font->m_fontAtlas = RetrieveFontTextureAtlas(fontFilepath, header.fontAtlasFileName);
		font->m_topPadding = header.padding[0];
		font->m_leftPadding = header.padding[1];
		font->m_bottomPadding = header.padding[2];
		font->m_rightPadding = header.padding[3];

		ADynArr<CharacterData> data(header.charCount);

		while (fontFile.peek() != EOF)
		{
			std::string currLine = ReadLine(fontFile);
			data.Add(ProcessChar(currLine));
		}

		for (CharacterData& c : data)
		{
			font->m_characters.Add(c.id, TextCharacter(font->m_fontAtlas, c.x, c.y, c.width, c.height, 
				c.offsetX, c.offsetY, c.xAdvance));
		}

		return font;
	}

	//TextElement//////////////////////////////////////

	TextElement::TextElement() : RenderableUIElement(Vector2::Zero(), 100, 100) { }
	TextElement::TextElement(const std::string& text) 
		: RenderableUIElement(Vector2::Zero(), 100, 100), m_text(text) { }
	TextElement::TextElement(AReference<Font>& font) 
		: RenderableUIElement(Vector2::Zero(), 100, 100), m_font(font) { }
	TextElement::TextElement(AReference<Font>& font, const std::string& text) 
		: RenderableUIElement(Vector2::Zero(), 100, 100), m_text(text), m_font(font) { }
	TextElement::TextElement(const Vector2& pos, size_t width, size_t height) 
		: RenderableUIElement(pos, width, height) { }
	TextElement::TextElement(const Vector2& pos, size_t width, size_t height, const std::string& text) 
		: RenderableUIElement(pos, width, height), m_text(text) { }
	TextElement::TextElement(const Vector2& pos, size_t width, size_t height,
		AReference<Font>& font, const std::string& text) : RenderableUIElement(pos, width, height), 
		m_text(text), m_font(font) { }

	static bool IsWhiteSpace(char c)
	{
		return c == ' ' || c == '\t' || c == '\n';
	}

	void TextElement::Draw() const
	{
		float fontScale = 0.5f;

		Vector2 virtualCursorPos = Vector2(-(float)GetScreenCoordsWidth() / 2.0f, 
			(-(float)GetScreenCoordsHeight()/* + m_font->GetLineHeight() * fontScale*/) / 2.0f);

		size_t currPos = 0;

		while(currPos < m_text.length())
		{
			size_t nextWhiteSpace = FindNextWhiteSpace(currPos);

			std::string_view currWord = std::string_view(m_text.c_str() + currPos, nextWhiteSpace - currPos);
			currPos = nextWhiteSpace + 1;

			size_t wordWidth = GetWordWidth(currWord, fontScale);

			if (ShouldChangeLines(Vector2(virtualCursorPos.x + wordWidth, virtualCursorPos.y), fontScale))
			{
				virtualCursorPos = ChangeLines(virtualCursorPos, fontScale);
			}

			virtualCursorPos = RenderWord(currWord, virtualCursorPos, fontScale);
			virtualCursorPos = SkipWhiteSpace(m_text[currPos - 1], virtualCursorPos, fontScale);
		}

		Renderer::DrawQuad(GetWorldPos(), 0.0f, Vector3(GetWorldWidth(), GetWorldHeight(), 0.0f)); //temp
	}

	Vector2 TextElement::RenderWord(std::string_view& word, Vector2 virtualCursorPos, float fontScale) const
	{
		for (char c : word)
		{
			const TextCharacter& character = m_font->GetCharacter(c);

			//The virtual cursor is positioned near the top of each letter/character so there is no need 
			//to invert the y axis (in screen coordinates greater values of y are lower on the screen)
			Vector2 characterScreenCoords = GetScreenCoords() + virtualCursorPos
				+ ((Vector2)character.GetOffset()) * fontScale
				+ Vector2((float)character.GetScreenCoordsWidth() * fontScale / 2.0f,
					(float)character.GetScreenCoordsHeight() * fontScale / 2.0f);

			AE_ERROR("function no longer exist");
			
			/*
			Renderer::DrawQuad((Vector3)ScreenToWorldCoords(characterScreenCoords), 0.0f,
				Vector3(fontScale * character.GetWorldWidth(), fontScale * character.GetWorldHeight(), 1.0f),
				m_font->GetFontAtlas(), character.GetTextureCoords().data(), 1.0f, m_color, true);
			*/

			virtualCursorPos.x += character.GetXAdvance() * fontScale;
		}

		return virtualCursorPos;
	}

	size_t TextElement::FindNextWhiteSpace(size_t offset) const
	{
		size_t nextWhiteSpace = m_text.find(' ', offset);

		{
			size_t tabPos = m_text.find('\t', offset);
			if (tabPos < nextWhiteSpace)
			{
				nextWhiteSpace = tabPos;
			}
		}

		{
			size_t newLinePos = m_text.find('\n', offset);
			if (newLinePos < nextWhiteSpace)
			{
				nextWhiteSpace = newLinePos;
			}
		}

		if (nextWhiteSpace == std::string::npos)
		{
			nextWhiteSpace = m_text.length();
		}

		return nextWhiteSpace;
	}

	Vector2 TextElement::SkipWhiteSpace(char whiteSpace, const Vector2& cursorPos, float fontScale) const
	{
		if (whiteSpace == '\0')
		{
			return cursorPos;
		}

		AE_CORE_ASSERT(IsWhiteSpace(whiteSpace), "Character provided \'%c\' is not a white space", whiteSpace);
		
		if (whiteSpace == '\n')
		{
			return ChangeLines(cursorPos, fontScale);
		}
		else if (whiteSpace == '\t')
		{
			Vector2 newCursorPos = Vector2(cursorPos.x + m_font->GetCharacter(' ').GetXAdvance() 
				* fontScale * Application::GetUIContext()->GetNumSpacesPerTab(), cursorPos.y);
			return newCursorPos;
		}

		if (m_font->HasCharacter(whiteSpace))
		{
			Vector2 newCursorPos = Vector2(cursorPos.x + m_font->GetCharacter(whiteSpace).GetXAdvance() * fontScale,
				cursorPos.y);
			return newCursorPos;
		}
		AE_CORE_ERROR("Unknown characters not yet handled");
		return cursorPos;
	}

	size_t TextElement::GetWordWidth(std::string_view& word, float fontScale) const
	{
		size_t width = 0;
		for (char c : word)
		{
			width += m_font->GetCharacter(c).GetXAdvance() * fontScale;
		}
		return width;
	}

	//Updates the virtual cursor position to change lines for the text
	Vector2 TextElement::ChangeLines(const Vector2& cursorPos, float fontScale) const
	{
		//Temp implementation, will need to consider centering
		return Vector2(-(float)GetScreenCoordsWidth() / 2.0f, cursorPos.y + m_font->GetLineHeight() * fontScale);
	}

	bool TextElement::ShouldChangeLines(const Vector2& cursorPos, float fontScale) const
	{
		return cursorPos.x > GetScreenCoordsWidth() / 2.0f;
	}
}