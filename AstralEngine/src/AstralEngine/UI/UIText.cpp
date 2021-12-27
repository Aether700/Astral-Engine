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
	TextCharacter::TextCharacter(AReference<Texture2D>& fontTextureAtlas, int topLeftCornerX, int topLeftCornerY, int width,
		int height, int offsetX, int offsetY, int xAdvance) : UIElement(Vector2(topLeftCornerX + width * 0.5f, 
			topLeftCornerY + height * 0.5f), width, height), m_fontTextureAtlas(fontTextureAtlas), 
		m_topLeftCornerCoords(Vector2Int(topLeftCornerX, topLeftCornerY)), m_offset(Vector2Int(offsetX, offsetY)), 
		m_xAdvance(xAdvance) { }

	const std::array<Vector2, 4> TextCharacter::GetTextureCoords() const
	{
		const std::array<Vector2, 4> textureCoords = {
			Vector2((float)m_topLeftCornerCoords.x / (float)m_fontTextureAtlas->GetWidth(),
				((float)(m_fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y 
				- GetScreenCoordsHeight())) / (float)m_fontTextureAtlas->GetHeight()),
			Vector2(((float)(m_topLeftCornerCoords.x + GetScreenCoordsWidth())) / (float)m_fontTextureAtlas->GetWidth(),
				((float)(m_fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y 
					- GetScreenCoordsHeight())) / (float)m_fontTextureAtlas->GetHeight()),
			Vector2(((float)(m_topLeftCornerCoords.x + GetScreenCoordsWidth())) / (float)m_fontTextureAtlas->GetWidth(),
				(float)(m_fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y) 
				/ (float)m_fontTextureAtlas->GetHeight()),
			Vector2((float)m_topLeftCornerCoords.x / (float)m_fontTextureAtlas->GetWidth(),
				(float)(m_fontTextureAtlas->GetHeight() - m_topLeftCornerCoords.y) 
				/ (float)m_fontTextureAtlas->GetHeight())
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

	static AReference<Texture2D> RetrieveFontTextureAtlas(const std::string& fontFilepath, 
		const std::string& fontAtlasFileName)
	{
		size_t lastSlash = fontFilepath.find_last_of("/");

		if (lastSlash == std::string::npos)
		{
			return Texture2D::Create(fontAtlasFileName);
		}

		std::stringstream ss;
		ss << fontFilepath.substr(0, lastSlash + 1);
		ss << fontAtlasFileName;

		return Texture2D::Create(ss.str());
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

	void TextElement::Draw() const
	{
		//temp implementation needs some tweaking
		Vector2 virtualCursorPos = Vector2::Zero();

		for (char c : m_text)
		{
			const TextCharacter& character = m_font->GetCharacter(c);

			/*
			[]._|->*[]
			[]._|->[.]
			*/

			Vector2 characterScreenCoords = GetScreenCoords() + virtualCursorPos + character.GetOffset()
				+ Vector2((((float)character.GetScreenCoordsWidth()) / 2.0f),
					-(((float)character.GetOffset().y) / 2.0f));
			/*
			Vector2 characterScreenCoords = GetScreenCoords() + m_virtualCursorPos + character.GetOffset()
				+ Vector2((((float)character.GetScreenCoordsWidth()) / 2.0f), 
					-(((float)character.GetOffset().y) / 2.0f));
			*/

			float fontScale = 1.0f;
			
			Renderer::DrawQuad((Vector3)ScreenToWorldCoords(characterScreenCoords), 0.0f,
				Vector3(fontScale * character.GetWorldWidth(), fontScale * character.GetWorldHeight(), 1.0f), 
				m_font->GetFontAtlas(), character.GetTextureCoords().data(), 1.0f, m_color, true);
			
			virtualCursorPos.x += character.GetXAdvance();
		}
	}
}