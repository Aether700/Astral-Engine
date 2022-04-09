#pragma once
#include <cstdint>

namespace AstralEngine
{
	enum ComponentGlyphsFlags : std::uint16_t
	{
		ComponentGlyphsFlagsArgs1And2AreWords = 0,
		ComponentGlyphsFlagsArgsAreXYValues = 1 << 1,
		ComponentGlyphsFlagsRoundXYToGrid = 1 << 2,
		ComponentGlyphsFlagsHasScale = 1 << 3,

		// skip 4th bit, is obsolete
		ComponentGlyphsFlagsMoreComponents = 1 << 5,
		ComponentGlyphsFlagsHasXYScale = 1 << 6,
		ComponentGlyphsFlagsIs2By2 = 1 << 7,
		ComponentGlyphsFlagsHasInstructions = 1 << 8,
		ComponentGlyphsFlagsUsesComponentMetric = 1 << 9,
		ComponentGlyphsFlagsOverlapCompound = 1 << 10
	};

	struct ComponentGlyph
	{
	public:
		ComponentGlyphsFlags flags;
		std::uint16_t glyphIndex;
		std::int16_t arg1;
		std::int16_t arg2;
	};

	struct Glyph
	{
	public:
		std::int16_t numberOfContours;
		std::int16_t xMin;
		std::int16_t yMin;
		std::int16_t xMax;
		std::int16_t yMax;
	};
}