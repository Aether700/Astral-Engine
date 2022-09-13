#pragma once
#include "AstralEngine/UI/UIText.h"

namespace AstralEngine
{
	class TTFParser
	{
	public:
		static AReference<Font> LoadFont(const std::string& filepath);
	};

	// temp
	class Glyph;

	class DebugTTFFont : public Font
	{
	public:
		void DebugDrawGlyph();

		ADynArr<Glyph> glyphs;
	};
}