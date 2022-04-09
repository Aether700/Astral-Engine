#include "aepch.h"
#include "TTFParser.h"

namespace AstralEngine
{
	struct OffsetSubtable
	{
		std::uint32_t scalerType;
		std::uint16_t numTables;
		std::uint16_t searchRange;
		std::uint16_t entrySelector;
		std::uint16_t rangeShift;
	};

	//one per table
	struct TableDirectory
	{
		std::uint32_t tag;
		//detail on how to verify checkSum at 
		//https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#ScalerTypeNote
		std::uint32_t checkSum; 
		std::uint32_t offset;
		std::uint32_t length;
	};

	struct CmapIndex
	{
		std::uint16_t version;
		std::uint16_t numSubtables;
	};

	enum class CmapPlatforms : std::uint16_t
	{
		Unicode = 0,
		Macintosh = 1,
		Reserved = 2, //do not use
		Microsoft = 3
	};

	enum class UnicodeEncoding : std::uint16_t
	{
		Version10 = 0, // for version 1.0
		Version11 = 1, // for version 1.1
		ISO_10646_1993 = 2, //deprecated
		Unicode2OrLaterBmpOnly = 3,
		Unicode2OrLaterNoBmpAllowed = 4,
		UnicodeVariationSequences = 5,
		LastResort = 6
	};

	enum class WindowEncoding : std::uint16_t
	{
		Symbol = 0,
		UnicodeBmpOnly = 1,
		ShiftJIS = 2,
		PRC = 3,
		BigFive = 4,
		Johab = 5,
		UnicodeUCS4 = 10
	};

	struct CmapSubtable
	{
		CmapPlatforms platformID;
		std::uint16_t platformSpecificEncoding;
		std::uint32_t offset;
	};

	struct CmapFormat4
	{
		std::uint16_t format;
		std::uint16_t length;
		std::uint16_t language;
		std::uint16_t segCountX2; // segCount * 2
		std::uint16_t searchRange;
		std::uint16_t entrySelector;
		std::uint16_t rangeShift;
		std::uint16_t endCode; // the end character for each segment
		std::uint16_t reservedPad; // should be 0
		std::uint16_t startCode; // starting character for each segment
		std::uint16_t idDelta; // delta for every character code in segment
		std::uint16_t idRangeOffset; // offset in bytes to glyph indexArray or 0
		std::uint16_t glyphIndexArray;
	};


	//implement 'head' table next

}