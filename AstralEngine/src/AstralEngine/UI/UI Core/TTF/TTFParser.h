#pragma once
#include "AstralEngine/UI/UIText.h"

namespace AstralEngine
{
	class TTFParser
	{
	public:
		static AReference<Font> LoadFont(const std::string& filepath);
	};
}