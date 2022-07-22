#include "aepch.h"
#include "TTFParser.h"

namespace AstralEngine
{
	// tags
	
	static constexpr std::uint32_t s_headTag = 0x68656164;
	static constexpr std::uint32_t s_hheaTag = 0x68686561;
	static constexpr std::uint32_t s_hmtxTag = 0x686D7478;
	static constexpr std::uint32_t s_maxpTag = 0x6D617870;

	//part of the font directory which is the first table of the ttf file
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


	struct HeaderTable // head
	{
		std::int16_t version;
		std::int16_t fontRevision;
		std::uint32_t checkSumAdjustment;
		std::uint32_t magicNumber;
		std::uint16_t flags;
		std::uint16_t unitsPerEm;
		std::int64_t created;
		std::int64_t modified;
		std::int16_t xMin;
		std::int16_t yMin;
		std::int16_t xMax;
		std::int16_t yMax;
		std::uint16_t macStyle;
		std::uint16_t lowestRecPPEM;
		std::int16_t fontDirectionHint;
		std::int16_t indexToLocFormat;
		std::int16_t glyphDataFormat;
	};

	struct HorizontalHeader // hhea
	{
		std::int16_t version;
		std::int16_t ascent;
		std::int16_t descent;
		std::int16_t lineGap;
		std::uint16_t advanceWidthMax;
		std::int16_t minLeftSideBearing;
		std::int16_t minRightSideBearing;
		std::int16_t xMaxExtent;
		std::int16_t caretSlopeRise;
		std::int16_t caretSlopeRun;
		std::int16_t caretOffset;
		std::int64_t reserved; //set to 0
		std::int16_t metricDataFormat; // 0 for current format
		std::uint16_t numOfLongHorMetrics;
	};

	// used in the hmtx table. The hmtx table is an array of LongHorizontalMetric structs 
	// with an optional array (this optional array is ommitted here)
	struct LongHorizontalMetric
	{
		std::uint16_t advanceWidth;
		std::int16_t leftSideBearing;

		bool operator==(const LongHorizontalMetric& other) const
		{
			return advanceWidth == other.advanceWidth && leftSideBearing == other.leftSideBearing;
		}

		bool operator!=(const LongHorizontalMetric& other) const
		{
			return !(*this == other);
		}
	};

	// table used to track the memory requirements of the font
	struct Maxp // maxp table
	{
		std::int16_t version;
		std::uint16_t numGlyphs;
		std::uint16_t maxPoints; // max number of points in non-compound glyphs
		std::uint16_t maxContours; // max number of contours in non-compound glyphs
		std::uint16_t maxCompoundPoints;
		std::uint16_t maxCompoundContours;
		std::uint16_t maxZones; // set to 2
		std::uint16_t maxTwilightPoints; // points used in twilight zone
		std::uint16_t maxStorage;
		std::uint16_t maxFunctionDefs;
		std::uint16_t maxInstructionDefs;
		std::uint16_t maxStackElements; // max stack depth
		std::uint16_t maxSizeOfInstructions; // max size of instructions for a single glyph in bytes
		std::uint16_t maxComponentElement; // maximum number of simple glyphs used in a single component glyph
		
		// max recursion depth when defining compound glyphs. 
		// If there are only simple glyphs set to 0
		std::uint16_t maxComponentDepth; 
	};
	
	
	struct VerticalHeader // vhea
	{
		std::uint32_t version;
		std::int16_t vertTypoAscender;
		std::int16_t vertTypoDescender;
		std::int16_t vertTypoLineGap;
		std::int16_t advanceHeightMax;
		std::int16_t minTopSideBearing;
		std::int16_t minBottomSideBearing;
		std::int16_t yMaxExtent;
		std::int16_t caretSlopeRise;
		std::int16_t caretSlopeRun;
		std::int16_t caretOffset;
		std::int64_t reserved; // set to 0
		std::int16_t metricDataFormat; // set to 0
		std::uint16_t numOfLongVerMetrics;
	};


	// Read Table functions /////////////////////////////////////////////////////
	OffsetSubtable ReadOffsetSubtable(std::ifstream& file)
	{
		OffsetSubtable offset;

		std::uint32_t data;
		file.read((char*)&data, sizeof(offset.scalerType));
		AssertDataEndianness(&data, &offset.scalerType, sizeof(offset.scalerType), Endianness::BigEndian);

		file.read((char*)&data, sizeof(offset.numTables));
		AssertDataEndianness(&data, &offset.numTables, sizeof(offset.numTables), Endianness::BigEndian);

		file.read((char*)&data, sizeof(offset.searchRange));
		AssertDataEndianness(&data, &offset.searchRange, sizeof(offset.searchRange), Endianness::BigEndian);

		file.read((char*)&data, sizeof(offset.entrySelector));
		AssertDataEndianness(&data, &offset.entrySelector, sizeof(offset.entrySelector), Endianness::BigEndian);

		file.read((char*)&data, sizeof(offset.rangeShift));
		AssertDataEndianness(&data, &offset.rangeShift, sizeof(offset.rangeShift), Endianness::BigEndian);

		return offset;
	}

	TableDirectory ReadTableDir(std::ifstream& file)
	{
		TableDirectory dir;

		std::uint32_t data;
		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.tag, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.checkSum, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.offset, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &dir.length, sizeof(std::uint32_t), Endianness::BigEndian);
		
		return dir;
	}

	HeaderTable ReadHeaderTable(std::ifstream& file)
	{
		HeaderTable head;
		std::uint64_t data;
		file.read((char*) &data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.version, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.fontRevision, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &head.checkSumAdjustment, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &head.magicNumber, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &head.flags, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &head.unitsPerEm, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint64_t));
		AssertDataEndianness(&data, &head.created, sizeof(std::uint64_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint64_t));
		AssertDataEndianness(&data, &head.modified, sizeof(std::uint64_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.xMin, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.yMin, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.xMax, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.yMax, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &head.macStyle, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &head.lowestRecPPEM, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.fontDirectionHint, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.indexToLocFormat, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &head.glyphDataFormat, sizeof(std::int16_t), Endianness::BigEndian);

		return head;
	}

	HorizontalHeader ReadHorizontalHeader(std::ifstream& file)
	{
		HorizontalHeader hhea;
		std::uint64_t data;
		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.version, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.ascent, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.descent, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.lineGap, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &hhea.advanceWidthMax, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.minLeftSideBearing, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.minRightSideBearing, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.xMaxExtent, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.caretSlopeRise, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.caretSlopeRun, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.caretOffset, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int64_t));
		AssertDataEndianness(&data, &hhea.reserved, sizeof(std::int64_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &hhea.metricDataFormat, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &hhea.numOfLongHorMetrics, sizeof(std::uint16_t), Endianness::BigEndian);

		return hhea;
	}

	LongHorizontalMetric ReadLongHorMetric(std::ifstream& file)
	{
		LongHorizontalMetric longHorMetric;

		std::uint16_t data;
		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&longHorMetric.advanceWidth, &data, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&longHorMetric.leftSideBearing, &data, sizeof(std::int16_t), Endianness::BigEndian);

		return longHorMetric;
	}

	Maxp ReadMaxp(std::ifstream& file)
	{
		Maxp m;

		std::uint16_t data;
		file.read((char*)&data, sizeof(std::int16_t));
		AssertDataEndianness(&data, &m.version, sizeof(std::int16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.numGlyphs, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxPoints, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxContours, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxCompoundPoints, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxCompoundContours, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxZones, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxTwilightPoints, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxStorage, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxFunctionDefs, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxInstructionDefs, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxStackElements, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxSizeOfInstructions, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxComponentElement, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &m.maxComponentDepth, sizeof(std::uint16_t), Endianness::BigEndian);

		return m;
	}

	// returns true if the check sum test was correct, false otherwise
	// do not use this function to validate the "head" table
	bool ValidateCheckSum(std::uint32_t* table, std::uint32_t tableSize, std::uint32_t targetCheckSum)
	{
		// checksums might be invalid as many fonts build their checksum differently than per specifications
		// look at https://stackoverflow.com/questions/54151857/missing-pieces-for-ttf-font-building-from-spec
		// for more info on how to properly verify the checksum of the font

		std::uint32_t calculatedCheckSum = 0;
		std::uint32_t numLongsInTable = (tableSize + 3) / 4;
		while (numLongsInTable > 0)
		{
			numLongsInTable--;
			calculatedCheckSum += *table;
			table++;
		}

		return calculatedCheckSum == targetCheckSum;
	}

	// TTFParser //////////////////////////////////////////////////////////


	// to double check table data: https://fontdrop.info/#/?darkmode=true go to tab "data"
	AReference<Font> TTFParser::LoadFont(const std::string& filepath)
	{
		std::ifstream file = std::ifstream(filepath, std::ios_base::binary);
		if (!file)
		{
			AE_CORE_WARN("Could not open file %S", filepath);
			return nullptr;
		}

		OffsetSubtable offsetSubtable = ReadOffsetSubtable(file);
		TableDirectory glyphOffsetTableDir = ReadTableDir(file);

		bool hheaInitialized = false;
		HeaderTable head;
		HorizontalHeader hhea;
		ADynArr<LongHorizontalMetric> hmtx;
		Maxp max;

		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			TableDirectory dir = ReadTableDir(file);

			// temp ///////////////////////////////////////
			char tableID[5];
			memcpy(tableID, (std::uint32_t*)&dir.tag, 4);
			tableID[4] = '\0';
			///////////////////////////////////////////////

			size_t oldPos = file.tellg();

			if (dir.tag == s_headTag)
			{
				file.seekg(dir.offset);
				head = ReadHeaderTable(file);
			}
			else if (dir.tag == s_hheaTag)
			{
				file.seekg(dir.offset);
				hhea = ReadHorizontalHeader(file);
				hmtx.Reserve(hhea.numOfLongHorMetrics);
				hheaInitialized = true;
			}
			else if (dir.tag == s_hmtxTag)
			{
				if (!hheaInitialized)
				{
					AE_CORE_ERROR("hhea table was not found before reaching the hmtx table");
					return nullptr;
				}

				file.seekg(dir.offset);
				for (size_t i = 0; i < (size_t)hhea.numOfLongHorMetrics; i++)
				{
					hmtx.Add(ReadLongHorMetric(file));
				}
			}
			else if (dir.tag == s_maxpTag)
			{
				file.seekg(dir.offset);
				max = ReadMaxp(file);
			}

			file.seekg(oldPos);
		}

		// temp
		return nullptr;
	}

}