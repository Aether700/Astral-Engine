#include "aepch.h"
#include "TTFParser.h"

namespace AstralEngine
{
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

	//do hmtx table next

	TableDirectory ReadTableDir(std::ifstream& file)
	{
		TableDirectory dir;
		std::uint32_t data;
		file.read((char*)&data, sizeof(data));
		AssertDataEndianness(&data, &dir.tag, sizeof(data), Endianness::BigEndian);

		file.read((char*)&data, sizeof(data));
		AssertDataEndianness(&data, &dir.checkSum, sizeof(data), Endianness::BigEndian);

		file.read((char*)&data, sizeof(data));
		AssertDataEndianness(&data, &dir.offset, sizeof(data), Endianness::BigEndian);

		file.read((char*)&data, sizeof(data));
		AssertDataEndianness(&data, &dir.length, sizeof(data), Endianness::BigEndian);

		return dir;
	}

	template<typename T>
	T ReadDataFromTTFFile(std::ifstream& file)
	{
		constexpr size_t dataSize = sizeof(T);
		std::uint8_t data[dataSize];
		file.read((char*) data, dataSize);
		std::uint8_t assertedEndiannessData[dataSize];
		// TTF files are always in big endian
		AssertDataEndianness(data, assertedEndiannessData, dataSize, Endianness::BigEndian); 
		T dataObj = *(T*)assertedEndiannessData;
		return dataObj;
	}

	// TTFParser //////////////////////////////////////////////////////////

	AReference<Font> TTFParser::LoadFont(const std::string& filepath)
	{
		std::ifstream file = std::ifstream(filepath, std::ios_base::binary);
		if (!file)
		{
			AE_CORE_WARN("Could not open file %S", filepath);
			return nullptr;
		}

		OffsetSubtable offsetSubtable = ReadDataFromTTFFile<OffsetSubtable>(file);
		TableDirectory glyphOffsetTableDir = ReadDataFromTTFFile<TableDirectory>(file);

		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			TableDirectory dir = ReadDataFromTTFFile<TableDirectory>(file);
			//TableDirectory dir = ReadTableDir(file);
			char tableID[5];
			memcpy(tableID, (std::uint32_t*)&dir.tag, 4);
			tableID[4] = '\0';
			size_t oldPos = file.tellg();

			if (strcmp(tableID, "head") == 0)
			{
				file.seekg(dir.offset);
				HeaderTable head = ReadDataFromTTFFile<HeaderTable>(file);
			}
		}

		// temp
		return nullptr;
	}

}