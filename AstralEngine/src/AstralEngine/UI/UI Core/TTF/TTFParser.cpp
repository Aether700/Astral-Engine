#include "aepch.h"
#include "TTFParser.h"

namespace AstralEngine
{
	// ttf variable types
	typedef std::int16_t shortFrac;
	typedef std::int16_t Fixed;
	typedef std::int16_t FWord;
	typedef std::uint16_t uFWord;
	typedef std::int16_t F2Dot14;
	typedef std::int64_t longDateTime;

	// tags
	
	static constexpr std::uint32_t s_headTag = 0x68656164;
	static constexpr std::uint32_t s_hheaTag = 0x68686561;
	static constexpr std::uint32_t s_hmtxTag = 0x686D7478;
	static constexpr std::uint32_t s_maxpTag = 0x6D617870;
	static constexpr std::uint32_t s_cmapTag = 0x636D6170;
	static constexpr std::uint32_t s_glyfTag = 0x676C7966;

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

		bool operator==(const TableDirectory& other) const
		{
			return tag == other.tag;
		}

		bool operator!=(const TableDirectory& other) const
		{
			return !(*this == other);
		}
	};

	struct HeaderTable // head
	{
		Fixed version;
		Fixed fontRevision;
		std::uint32_t checkSumAdjustment;
		std::uint32_t magicNumber;
		std::uint16_t flags;
		std::uint16_t unitsPerEm;
		longDateTime created;
		longDateTime modified;
		FWord xMin;
		FWord yMin;
		FWord xMax;
		FWord yMax;
		std::uint16_t macStyle;
		std::uint16_t lowestRecPPEM;
		std::int16_t fontDirectionHint;
		std::int16_t indexToLocFormat;
		std::int16_t glyphDataFormat;
	};

	struct HorizontalHeader // hhea
	{
		Fixed version;
		FWord ascent;
		FWord descent;
		FWord lineGap;
		uFWord advanceWidthMax;
		FWord minLeftSideBearing;
		FWord minRightSideBearing;
		FWord xMaxExtent;
		std::int16_t caretSlopeRise;
		std::int16_t caretSlopeRun;
		FWord caretOffset;
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
	struct MaximumProfileTable // maxp table
	{
		Fixed version;
		std::uint16_t numGlyphs;
		std::uint16_t maxPoints; // max number of points in non-compound glyphs
		std::uint16_t maxContours; // max number of contours in non-compound glyphs
		std::uint16_t maxComponentPoints;
		std::uint16_t maxComponentContours;
		std::uint16_t maxZones; // set to 2
		std::uint16_t maxTwilightPoints; // points used in twilight zone
		std::uint16_t maxStorage;
		std::uint16_t maxFunctionDefs;
		std::uint16_t maxInstructionDefs;
		std::uint16_t maxStackElements; // max stack depth
		std::uint16_t maxSizeOfInstructions; // max size of instructions for a single glyph in bytes
		std::uint16_t maxComponentElements; // maximum number of simple glyphs used in a single component glyph
		
		// max recursion depth when defining compound glyphs. 
		// If there are only simple glyphs set to 0
		std::uint16_t maxComponentDepth; 
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

	struct CmapFormat
	{
	};


	struct CmapFormat4 : public CmapFormat
	{
		/* this format contains an array of glyphs which maps characters to their glyph 
		   however the array is not dense and might have indices with invalid characters 
		   (those characters map to index 0 which is the missing character glyph). A series 
		   of valid characters in that array is called a "segment" and we search through 
		   those segments to find a provided character quickly
		*/
		std::uint16_t format;
		std::uint16_t length;
		std::uint16_t language;
		std::uint16_t segCount; // segCount * 2
		std::uint16_t searchRange;
		std::uint16_t entrySelector;
		std::uint16_t rangeShift;
		std::uint16_t* endCode; // the end character for each segment
		std::uint16_t reservedPad; // should be 0
		std::uint16_t* startCode; // starting character for each segment
		std::int16_t* idDelta; // delta for every character code in segment
		std::uint16_t* idRangeOffset; // offset in bytes to glyph indexArray or 0
		std::uint16_t* glyphIndexArray;

		CmapFormat4() : endCode(nullptr), startCode(nullptr), idDelta(nullptr), 
			idRangeOffset(nullptr), glyphIndexArray(nullptr)
		{
		}

		~CmapFormat4()
		{
			delete[] endCode;
			delete[] startCode;
			delete[] idDelta;
			delete[] idRangeOffset;
			delete[] glyphIndexArray;
		}

		// returns the id of the glyph corresponding to the character 
		// provided or 0 if the character was not found
		std::uint16_t GetGlyphID(char c)
		{
			return GetGlyphID((wchar_t)c);
		}

		std::uint16_t GetGlyphID (wchar_t c)
		{
			std::uint16_t &charID = reinterpret_cast<std::uint16_t &>(c);

			// look through each segment to see if the character could be inside of it
			for (std::uint16_t i = 0; i < segCount; i++)
			{
				if (endCode[i] >= charID)
				{
					// verify that the character really is in this segment
					if (startCode[i] > charID)
					{
						break;
					}

					// the character is in this segment and we need to extract it. Equations taken from 
					// apple documentation: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6cmap.html

					if (idRangeOffset[i] == 0)
					{
						return (idDelta[i] + charID) % 65536;
					}
					else
					{
						std::uint16_t index = glyphIndexArray[i - segCount + idRangeOffset[i] / 2 
							+ (charID - startCode[i])];
						
						if (index == 0)
						{
							break;
						}
						return (idDelta[i] + index) % 65536;
					}
				}
			}

			return 0;
		}

	};

	struct Cmap
	{
		CmapIndex index;
		CmapSubtable* subtables;
		CmapFormat* format;

		Cmap() : subtables(nullptr), format(nullptr) { }
		Cmap(Cmap&& other) noexcept : index(other.index), subtables(other.subtables), format(other.format) 
		{
			other.subtables = nullptr;
			other.format = nullptr;
		}

		~Cmap()
		{
			delete[] subtables;
			delete format;
		}

		CmapSubtable* GetSubtable(CmapPlatforms platformID)
		{
			if (subtables == nullptr)
			{
				return nullptr;
			}

			for (std::uint16_t i = 0; i < index.numSubtables; i++)
			{
				if (subtables[i].platformID == platformID)
				{
					return &subtables[i];
				}
			}
			return nullptr;
		}

		Cmap& operator=(Cmap&& other)
		{
			delete[] subtables;
			delete format;

			index = other.index;
			format = other.format;
			subtables = other.subtables;

			other.subtables = nullptr;
			other.format = nullptr;

			return *this;
		}
	};
	
	enum TTFOutlineFlags : std::uint8_t // used for simple glyphs
	{
		TTFOutlineFlagsOnCurve = 1,
		TTFOutlineXShortVec = 1 << 1, // if set the coordinate is 1 byte long otherwise it is 2 bytes long
		TTFOutlineYShortVec = 1 << 2, // if set the coordinate is 1 byte long otherwise it is 2 bytes long
		
		// if set the next byte specifies the number of additional times this set 
		// of flags is to be repeated (the number of flags can be smaller than the 
		// number of points on the glyph)
		TTFOutlineRepeat = 1 << 3, 

		// meaning of the bit depends on the X/YShort bit. See documentation for details: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		TTFOutlineXSameOrPositive = 1 << 4,
		TTFOutlineYSameOrPositive = 1 << 5,
		TTFOutlineReserved1 = 1 << 6,
		TTFOutlineReserved2 = 1 << 7
	};

	struct GlyphData
	{
	};

	struct SimpleGlyphData : public GlyphData
	{
		union SimpleGlyphCoord
		{
			std::uint8_t bytes1;
			std::int16_t bytes2;
		}; // which type to use is specified by the flags
		
		std::uint16_t* endPtsOfContours; // array of length numberOfContours
		std::uint16_t instructionLength; // in bytes
		std::uint8_t* instructions; // array of length instructionLength
		TTFOutlineFlags* flags; // array of variable length
		std::uint8_t* repeatCounts; // for internal use only, not part of the documentation
		size_t numFlags;
		SimpleGlyphCoord* xCoordinates;
		size_t numXCoords;
		SimpleGlyphCoord* yCoordinates;
		size_t numYCoords;

		~SimpleGlyphData()
		{
			delete[] endPtsOfContours;
			delete[] instructions;
			delete[] xCoordinates;
			delete[] yCoordinates;
			delete[] repeatCounts;
		}
	};

	enum TTFCompoundGlyphComponentFlags : std::uint16_t
	{
		Args1And2AreWords = 1,
		ArgsAreXYValues = 1 << 1,
		RoundXYToGrid = 1 << 2,
		HasAScale = 1 << 3,
		Obsolete = 1 << 4, // ignore
		MoreComponents = 1 << 5,
		HasXYScale = 1 << 6,
		IsTwoByTwo = 1 << 7,
		HasInstructions = 1 << 8,
		UsesComponentMetric = 1 << 9,
		OverlapCompound = 1 << 10
	};

	struct CompoundGlyphData : public GlyphData
	{
		TTFCompoundGlyphComponentFlags flags;
		std::uint16_t glyphIndex;
		//not finished
	};

	struct GlyphDescription // used in glyf
	{
		std::int16_t numberOfContours; // if > 0-> simple glyph, if < 0 compound glyph if == 0 no glyph data
		FWord xMin;
		FWord yMin;
		FWord xMax;
		FWord yMax;
		GlyphData* data;

		GlyphDescription() : data(nullptr) { }
		GlyphDescription(GlyphDescription&& other) : data(other.data), numberOfContours(other.numberOfContours), 
			xMin(other.xMin), yMin(other.yMin), xMax(other.xMax), yMax(other.yMax) 
		{
			other.data = nullptr;
		}

		~GlyphDescription() 
		{
			delete data;
		}

		// TODO fix ADynArr to allow move operator only to be used
		GlyphDescription& operator=(const GlyphDescription& other)
		{
			AE_CORE_ERROR("Temporary function do not use");
			return *this; 
		}
		//////////////////////////////////////////////////////////

		GlyphDescription& operator=(GlyphDescription&& other) noexcept
		{
			delete data;
			data = other.data;
			other.data = nullptr;

			numberOfContours = other.numberOfContours;
			xMin = other.xMin;
			yMin = other.yMin;
			xMax = other.xMax;
			yMax = other.yMax;

			return *this;
		}

		bool operator==(const GlyphDescription& other) const
		{
			return numberOfContours == other.numberOfContours && data == other.data;
		}

		bool operator!=(const GlyphDescription& other) const
		{
			return !(*this == other);
		}
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


	// Read functions /////////////////////////////////////////////////////

	/* Variable Reader functions
	   these functions read the given variable from the provided file and then skips the 
	   padding as necessary. TTF files are 32 bit aligned and padded with 0s
	*/

	template<typename T>
	T ReadTTFVar(std::ifstream& file)
	{
		T data;
		file.read((char*)&data, sizeof(T));

		T var;
		AssertDataEndianness(&data, &var, sizeof(T), Endianness::BigEndian);
		return var;
	}

	template<typename T>
	T* ReadTTFArr(std::ifstream& file, size_t arraySize)
	{
		T* arr = new T[arraySize];

		for (size_t i = 0; i < arraySize; i++)
		{
			arr[i] = ReadTTFVar<T>(file);
		}

		return arr;
	}

	Fixed ReadFixed(std::ifstream& file)
	{
		Fixed data;
		file.read((char*)&data, sizeof(Fixed));
		file.seekg((size_t)file.tellg() + 2);

		Fixed var;
		AssertDataEndianness(&data, &var, sizeof(Fixed), Endianness::BigEndian);
		return var;
	}

	FWord ReadFWord(std::ifstream& file)
	{
		FWord data;
		file.read((char*)&data, sizeof(FWord));

		FWord var;
		AssertDataEndianness(&data, &var, sizeof(FWord), Endianness::BigEndian);
		return var;
	}
	
	uFWord ReadUFWord(std::ifstream& file)
	{
		uFWord data;
		file.read((char*)&data, sizeof(uFWord));

		uFWord var;
		AssertDataEndianness(&data, &var, sizeof(uFWord), Endianness::BigEndian);
		return var;
	}

	longDateTime ReadLongDateTime(std::ifstream& file)
	{
		longDateTime data;
		file.read((char*)&data, sizeof(longDateTime));

		longDateTime var;
		AssertDataEndianness(&data, &var, sizeof(longDateTime), Endianness::BigEndian);
		return var;
	}

	// Table Reader functions /////////////////////////////////////////////////////////////

	OffsetSubtable ReadOffsetSubtable(std::ifstream& file)
	{
		OffsetSubtable offset;

		std::uint32_t data;
		file.read((char*)&data, sizeof(std::uint32_t));
		AssertDataEndianness(&data, &offset.scalerType, sizeof(std::uint32_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.numTables, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.searchRange, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.entrySelector, sizeof(std::uint16_t), Endianness::BigEndian);

		file.read((char*)&data, sizeof(std::uint16_t));
		AssertDataEndianness(&data, &offset.rangeShift, sizeof(std::uint16_t), Endianness::BigEndian);

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

		head.version = ReadFixed(file);
		head.fontRevision = ReadFixed(file);
		
		head.checkSumAdjustment = ReadTTFVar<std::uint32_t>(file);
		head.magicNumber = ReadTTFVar<std::uint32_t>(file);
		head.flags = ReadTTFVar<std::uint16_t>(file);
		head.unitsPerEm = ReadTTFVar<std::uint16_t>(file);

		head.created = ReadLongDateTime(file);
		head.modified = ReadLongDateTime(file);

		head.xMin = ReadFWord(file);
		head.yMin = ReadFWord(file);
		head.xMax = ReadFWord(file);
		head.yMax = ReadFWord(file);

		head.macStyle = ReadTTFVar<std::uint16_t>(file);
		head.lowestRecPPEM = ReadTTFVar<std::uint16_t>(file);
		head.fontDirectionHint = ReadTTFVar<std::int16_t>(file);
		head.indexToLocFormat = ReadTTFVar<std::int16_t>(file);
		head.glyphDataFormat = ReadTTFVar<std::int16_t>(file);

		return head;
	}

	HorizontalHeader ReadHorizontalHeader(std::ifstream& file)
	{
		HorizontalHeader hhea;
		hhea.version = ReadFixed(file);
		hhea.ascent = ReadFWord(file);
		hhea.descent = ReadFWord(file);
		hhea.lineGap = ReadFWord(file);

		hhea.advanceWidthMax = ReadUFWord(file);
		hhea.minLeftSideBearing = ReadFWord(file);
		hhea.minRightSideBearing = ReadFWord(file);
		hhea.xMaxExtent = ReadFWord(file);

		hhea.caretSlopeRise = ReadTTFVar<std::int16_t>(file);
		hhea.caretSlopeRun = ReadTTFVar<std::int16_t>(file);

		hhea.caretOffset = ReadFWord(file);
		hhea.reserved = ReadTTFVar<std::int64_t>(file);
		hhea.metricDataFormat = ReadTTFVar<std::int16_t>(file);
		hhea.numOfLongHorMetrics = ReadTTFVar<std::uint16_t>(file);

		return hhea;
	}

	LongHorizontalMetric ReadLongHorMetric(std::ifstream& file)
	{
		LongHorizontalMetric longHorMetric;

		longHorMetric.advanceWidth = ReadTTFVar<std::uint16_t>(file);
		longHorMetric.leftSideBearing = ReadTTFVar<std::int16_t>(file);

		return longHorMetric;
	}

	MaximumProfileTable ReadMaximumProfileTable(std::ifstream& file)
	{
		MaximumProfileTable maxp;
		maxp.version = ReadFixed(file);
		maxp.numGlyphs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxPoints = ReadTTFVar<std::uint16_t>(file);
		maxp.maxContours = ReadTTFVar<std::uint16_t>(file);

		maxp.maxComponentPoints = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentContours = ReadTTFVar<std::uint16_t>(file);
		maxp.maxZones = ReadTTFVar<std::uint16_t>(file);
		maxp.maxTwilightPoints = ReadTTFVar<std::uint16_t>(file);

		maxp.maxStorage = ReadTTFVar<std::uint16_t>(file);
		maxp.maxFunctionDefs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxInstructionDefs = ReadTTFVar<std::uint16_t>(file);
		maxp.maxStackElements = ReadTTFVar<std::uint16_t>(file);

		maxp.maxSizeOfInstructions = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentElements = ReadTTFVar<std::uint16_t>(file);
		maxp.maxComponentDepth = ReadTTFVar<std::uint16_t>(file);

		return maxp;
	}

	CmapIndex ReadCmapIndex(std::ifstream& file)
	{
		CmapIndex index;
		index.version = ReadTTFVar<std::uint16_t>(file);
		index.numSubtables = ReadTTFVar<std::uint16_t>(file);

		return index;
	}

	CmapSubtable ReadCmapSubtable(std::ifstream& file)
	{
		CmapSubtable subtable;
		subtable.platformID = ReadTTFVar<CmapPlatforms>(file);
		subtable.platformSpecificEncoding = ReadTTFVar<std::uint16_t>(file);
		subtable.offset = ReadTTFVar<std::uint32_t>(file);

		return subtable;
	}

	CmapSubtable* ReadCmapSubtableArr(std::ifstream& file, std::uint16_t numSubtables)
	{
		CmapSubtable* subtables = new CmapSubtable[numSubtables];

		for (std::uint16_t i = 0; i < numSubtables; i++)
		{
			subtables[i] = ReadCmapSubtable(file);
		}

		return subtables;
	}

	CmapFormat4* ReadCmapFormat4(std::ifstream& file)
	{
		CmapFormat4* format4 = new CmapFormat4();
		format4->format = 4;
		format4->length = ReadTTFVar<std::uint16_t>(file);
		format4->language = ReadTTFVar<std::uint16_t>(file);
		format4->segCount = ReadTTFVar<std::uint16_t>(file) / 2;
		format4->searchRange = ReadTTFVar<std::uint16_t>(file);
		format4->entrySelector = ReadTTFVar<std::uint16_t>(file);
		format4->rangeShift = ReadTTFVar<std::uint16_t>(file);
		format4->endCode = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		format4->reservedPad = ReadTTFVar<std::uint16_t>(file);
		format4->startCode = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		format4->idDelta = ReadTTFArr<std::int16_t>(file, format4->segCount);
		format4->idRangeOffset = ReadTTFArr<std::uint16_t>(file, format4->segCount);
		size_t dataReadSoFar = 14 + 2 * 4 * format4->segCount; // data read for the Cmap format so far in bytes
		format4->glyphIndexArray = ReadTTFArr<std::uint16_t>(file, (format4->length - dataReadSoFar) / 2);

		return format4;
	}

	CmapFormat* ReadCmapFormat(std::ifstream& file)
	{
		std::uint16_t format = ReadTTFVar<std::uint16_t>(file);

		switch(format)
		{
		case 4:
			return ReadCmapFormat4(file);

		case 6:
			AE_CORE_WARN("Cmap format 6 is not yet supported");
			return nullptr;
		}

		AE_CORE_ERROR("Cmap format not supported");
		return nullptr;
	}

	Cmap ReadCmap(std::ifstream& file)
	{
		Cmap c;
		c.index = ReadCmapIndex(file);
		c.subtables = ReadCmapSubtableArr(file, c.index.numSubtables);
		c.format = ReadCmapFormat(file);
		return c;
	}

	SimpleGlyphData* ReadSimpleGlyphData(std::ifstream& file, std::int16_t numContours)
	{
		AE_CORE_ASSERT(numContours > 0, "Number of contours cannot be <= 0 for a simple glyph");
		

		SimpleGlyphData* data = new SimpleGlyphData();
		AE_CORE_ASSERT(file.good(), "");
		data->endPtsOfContours = ReadTTFArr<std::uint16_t>(file, (size_t)numContours); 
		AE_CORE_ASSERT(file.good(), "");
		data->instructionLength = ReadTTFVar<std::uint16_t>(file);
		AE_CORE_ASSERT(file.good(), "");
		data->instructions = ReadTTFArr<std::uint8_t>(file, (size_t)data->instructionLength); 
		AE_CORE_ASSERT(file.good(), "");

		// read flags
		size_t flagIndex = 0;
		TTFOutlineFlags* tempArr = new TTFOutlineFlags[numContours];
		std::uint8_t* tempRepeatArr = new std::uint8_t[numContours];

		for (std::int16_t i = 0; i < numContours; i++)
		{
			TTFOutlineFlags currFlag = ReadTTFVar<TTFOutlineFlags>(file);
			tempArr[flagIndex] = currFlag;

			if (currFlag & TTFOutlineRepeat)
			{
				tempRepeatArr[flagIndex] = ReadTTFVar<std::uint8_t>(file);
				i += tempRepeatArr[flagIndex];
			}
			else
			{
				tempRepeatArr[flagIndex] = 0;
			}
			flagIndex++;
		}

		data->numFlags = flagIndex;
		data->flags = new TTFOutlineFlags[data->numFlags];
		data->repeatCounts = new std::uint8_t[data->numFlags];

		for (size_t i = 0; i < data->numFlags; i++)
		{
			data->flags[i] = tempArr[i];
			data->repeatCounts[i] = tempRepeatArr[i];
		}

		delete[] tempArr;
		delete[] tempRepeatArr;

		SimpleGlyphData::SimpleGlyphCoord* tempCoordArr = new SimpleGlyphData::SimpleGlyphCoord[data->numFlags];
		size_t coordIndex = 0;

		AE_CORE_ASSERT(file.good(), "");
		// read X coords
		for (size_t i = 0; i < data->numFlags; i++)
		{
			TTFOutlineFlags& currFlag = data->flags[i];
			if (currFlag & TTFOutlineXShortVec)
			{
				tempCoordArr[coordIndex].bytes1 = ReadTTFVar<std::uint8_t>(file);
				coordIndex++;
			}
			else if (!(currFlag & TTFOutlineXSameOrPositive))
			{
				tempCoordArr[coordIndex].bytes2 = ReadTTFVar<std::int16_t>(file);
				coordIndex++;
			}
		}

		data->numXCoords = coordIndex;
		data->xCoordinates = new SimpleGlyphData::SimpleGlyphCoord[data->numXCoords];

		for (size_t i = 0; i < data->numXCoords; i++)
		{
			data->xCoordinates[i] = tempCoordArr[i];
		}

		AE_CORE_ASSERT(file.good(), "");
		// read y coords
		coordIndex = 0;
		for (size_t i = 0; i < data->numFlags; i++)
		{
			TTFOutlineFlags& currFlag = data->flags[i];
			if (currFlag & TTFOutlineYShortVec)
			{
				tempCoordArr[coordIndex].bytes1 = ReadTTFVar<std::uint8_t>(file);
				coordIndex++;
			}
			else if (!(currFlag & TTFOutlineYSameOrPositive))
			{
				tempCoordArr[coordIndex].bytes2 = ReadTTFVar<std::int16_t>(file);
				coordIndex++;
			}
		}

		data->numYCoords = coordIndex;
		data->yCoordinates = new SimpleGlyphData::SimpleGlyphCoord[data->numYCoords];

		for (size_t i = 0; i < data->numYCoords; i++)
		{
			data->yCoordinates[i] = tempCoordArr[i];
		}

		delete[] tempCoordArr;
		AE_CORE_ASSERT(file.good(), ""); // temp for debug purposes

		return data;
	}

	CompoundGlyphData* ReadCompoundGlyphData(std::ifstream& file)
	{
		return nullptr;
	}

	GlyphDescription ReadGlyphDescription(std::ifstream& file)
	{
		AE_CORE_ASSERT(file.good(), "");

		GlyphDescription glyph;
		
		glyph.numberOfContours = ReadTTFVar<std::int16_t>(file); this value is not being read properly
		glyph.xMin = ReadFWord(file);
		glyph.yMin = ReadFWord(file);
		glyph.xMax = ReadFWord(file);
		glyph.yMax = ReadFWord(file);
		
		if (glyph.numberOfContours > 0)
		{
			glyph.data = ReadSimpleGlyphData(file, glyph.numberOfContours);
		}
		else if (glyph.numberOfContours < 0)
		{
			glyph.data = ReadCompoundGlyphData(file);
		}
		// else there is no data so leave data to nullptr (set by constructor)

		return glyph;
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

	TableDirectory* FindTable(ASinglyLinkedList<TableDirectory*>& tableList, std::uint32_t tag)
	{
		for (TableDirectory* currDir : tableList)
		{
			if (currDir->tag == tag)
			{
				return currDir;
			}
		}
		return nullptr;
	}

	// TTFParser //////////////////////////////////////////////////////////

	//temp debug functions//////////////////
	void PrintTableTag(TableDirectory& dir)
	{
		char tableID[5];
		AssertDataEndianness((std::uint32_t*)&dir.tag, tableID, 4, Endianness::BigEndian);
		tableID[4] = '\0';
		std::cout << tableID << "\n";
	}

	void LoopGlyphDescription(ADynArr<GlyphDescription>& glyf)
	{
		for (GlyphDescription& g : glyf)
		{
			if (g.data == nullptr)
			{
				int x = 0;
			}
			else if (g.numberOfContours > 0)
			{
				SimpleGlyphData* simple = (SimpleGlyphData*)g.data;
				int x = 0;
			}
			else
			{
				int x = 0;
			}
		}
	}

	///////////////////////////////////////


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

		HeaderTable head;
		HorizontalHeader hhea;
		ADynArr<LongHorizontalMetric> hmtx;
		ADynArr<GlyphDescription> glyf;
		MaximumProfileTable maxp;
		Cmap cmap;

		ASinglyLinkedList<TableDirectory> tableDirectories;
		ASinglyLinkedList<TableDirectory*> dependencyTables; // tables required by other tables for initialization

		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			tableDirectories.Add(ReadTableDir(file));
			TableDirectory* currDir = &tableDirectories[0];
			switch(currDir->tag)
			{
			case s_hheaTag:
			case s_maxpTag:
				dependencyTables.Add(currDir);
				break;
			}
		}

		for (TableDirectory* dir : dependencyTables)
		{
			size_t oldPos = file.tellg();
			file.seekg(dir->offset);
			switch(dir->tag)
			{
			case s_hheaTag:
				hhea = ReadHorizontalHeader(file);
				break;

			case s_maxpTag:
				maxp = ReadMaximumProfileTable(file);
				break;
			}
			file.seekg(oldPos);
		}

		for (TableDirectory& dir : tableDirectories)
		{
			if (FindTable(dependencyTables, dir.tag) != nullptr)
			{
				continue;
			}
			size_t oldPos = file.tellg();
			file.seekg(dir.offset);

			PrintTableTag(dir);

			switch(dir.tag)
			{
			case s_headTag:
				head = ReadHeaderTable(file);
				break;

			case s_hmtxTag:
				hmtx.Reserve(hhea.numOfLongHorMetrics);
				for (size_t i = 0; i < (size_t)hhea.numOfLongHorMetrics; i++)
				{
					hmtx.Add(ReadLongHorMetric(file));
				}
				break;

			case s_cmapTag:
				cmap = std::move(ReadCmap(file));
				break;
				
			case s_glyfTag:
				glyf.Reserve(maxp.numGlyphs);
				for (size_t i = 0; i < maxp.numGlyphs; i++)
				{
					glyf.Add(std::move(ReadGlyphDescription(file)));
				}
				LoopGlyphDescription(glyf); // temp
				break;
			}
			file.seekg(oldPos);
		}
	

		/*
		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			TableDirectory dir = ReadTableDir(file);

			// temp ///////////////////////////////////////
			char tableID[5];
			memcpy(tableID, (std::uint32_t*)&dir.tag, 4);
			tableID[4] = '\0';
			///////////////////////////////////////////////

			size_t oldPos = file.tellg();
			file.seekg(dir.offset);

			if (dir.tag == s_headTag)
			{
				head = ReadHeaderTable(file);
			}
			else if (dir.tag == s_hheaTag)
			{
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

				for (size_t i = 0; i < (size_t)hhea.numOfLongHorMetrics; i++)
				{
					hmtx.Add(ReadLongHorMetric(file));
				}
			}
			else if (dir.tag == s_maxpTag)
			{
				maxp = ReadMaximumProfileTable(file);
				glyf.Reserve(maxp.numGlyphs);
				maxpInitialized = true;
			}
			else if (dir.tag == s_cmapTag)
			{
				cmap = std::move(ReadCmap(file));
			}
			else if (dir.tag == s_glyfTag)
			{
				AE_CORE_ASSERT(maxpInitialized, "Maximum profile table not initialized before reading the glyf table");

				GlyphDescription des = ReadGlyphDescription(file);
				int x = 0;
				/*
				for (size_t i = 0; i < maxp.numGlyphs; i++)
				{
					glyf.Add(ReadGlyphDescription(file));
				}
				/
			}


			file.seekg(oldPos);
		}
		*/



		// temp
		return nullptr;
	}

}