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
	
	static constexpr std::uint32_t s_headTag = 0x64616568;
	static constexpr std::uint32_t s_hheaTag = 0x61656868;
	static constexpr std::uint32_t s_hmtxTag = 0x78746D68;
	static constexpr std::uint32_t s_maxpTag = 0x7078616D;
	static constexpr std::uint32_t s_cmapTag = 0x70616D63;
	static constexpr std::uint32_t s_glyfTag = 0x66796C67;
	static constexpr std::uint32_t s_locaTag = 0x61636F6C;

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

	struct IndexToLocationTable //loca
	{
		bool isShort;
		union OffsetArr
		{
			std::uint16_t* shortArr;
			std::uint32_t* longArr;
		} offsetArr;
		size_t arrLen;

		IndexToLocationTable() : isShort(true), arrLen(0)
		{
			offsetArr.shortArr = nullptr;
		}

		IndexToLocationTable(IndexToLocationTable&& other) noexcept : isShort(other.isShort), arrLen(other.arrLen)
		{
			if (isShort)
			{
				offsetArr.shortArr = other.offsetArr.shortArr;
				other.offsetArr.shortArr = nullptr;
			}
			else
			{
				offsetArr.longArr = other.offsetArr.longArr;
				other.offsetArr.longArr = nullptr;
			}
			other.arrLen = 0;
		}

		~IndexToLocationTable()
		{
			if (isShort)
			{
				delete[] offsetArr.shortArr;
			}
			else
			{
				delete[] offsetArr.longArr;
			}
		}

		// retrieves the position of the glyph in the font file
		std::uint32_t GetGlyphOffset(size_t glyphIndex)
		{
			if (isShort)
			{
				return offsetArr.shortArr[glyphIndex] * 2;
			}
			return offsetArr.longArr[glyphIndex];
		}

		IndexToLocationTable& operator=(IndexToLocationTable&& other) noexcept
		{
			if (isShort)
			{
				delete[] offsetArr.shortArr;
			}
			else
			{
				delete[] offsetArr.longArr;
			}

			isShort = other.isShort;
			arrLen = other.arrLen;
			if (isShort)
			{
				offsetArr.shortArr = other.offsetArr.shortArr;
				other.offsetArr.shortArr = nullptr;
			}
			else
			{
				offsetArr.longArr = other.offsetArr.longArr;
				other.offsetArr.longArr = nullptr;
			}
			other.arrLen = 0;

			return *this;
		}
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
		SimpleGlyphCoord* xCoordinates;
		SimpleGlyphCoord* yCoordinates;

		// for internal use only, not part of the documentation
		std::uint8_t* repeatCounts; 
		size_t numOfContours;
		size_t numFlags;
		size_t numXCoords;
		size_t numYCoords;

		~SimpleGlyphData()
		{
			delete[] endPtsOfContours;
			delete[] instructions;
			delete[] xCoordinates;
			delete[] yCoordinates;
			delete[] repeatCounts;
		}

		TTFOutlineFlags GetFlags(size_t index)
		{
			AE_CORE_ASSERT(index < GetNumPoints(), "Index out of bounds");

			if (index >= numFlags)
			{
				return flags[numFlags - 1];
			}

			TTFOutlineFlags lastFlag = flags[0];
			for (size_t i = 0; i < index; i++)
			{
				lastFlag = flags[i];
				if (flags[i] & TTFOutlineRepeat)
				{
					i += repeatCounts[i];
				}
			}
			return lastFlag;
		}
		
		Vector2Int GetCoords(size_t index)
		{
			return Vector2Int(ReadCoordComponent(index, xCoordinates, TTFOutlineXShortVec, TTFOutlineXSameOrPositive), 
				ReadCoordComponent(index, yCoordinates, TTFOutlineYShortVec, TTFOutlineYSameOrPositive));
		}
		
		std::uint16_t GetNumPoints() const
		{
			return endPtsOfContours[numOfContours - 1];
		}

	private:
		int ReadCoordComponent(size_t index, SimpleGlyphCoord* coordArr, 
			TTFOutlineFlags shortVec, TTFOutlineFlags sameOrPos)
		{
			int coord = 0;
			size_t coordIndex = 0;
			for (size_t i = 0; i < index; i++)
			{
				TTFOutlineFlags currFlag = GetFlags(i);
				int currChange = 0;
				if (currFlag & shortVec)
				{
					currChange = (int)coordArr[coordIndex].bytes1;
					if (!(currFlag & sameOrPos))
					{
						currChange *= -1;
					}
					coord += currChange;
					coordIndex++;
				}
				else if (!(currFlag & sameOrPos))
				{
					coord += (int)coordArr[coordIndex].bytes2;
					coordIndex++;
				}
			}

			return coord;
		}
	};

	enum TTFCompoundGlyphComponentFlags : std::uint16_t
	{
		TTFCompoundGlyphComponentFlagsArgs1And2AreWords = 1,
		TTFCompoundGlyphComponentFlagsArgsAreXYValues = 1 << 1,
		TTFCompoundGlyphComponentFlagsRoundXYToGrid = 1 << 2,
		TTFCompoundGlyphComponentFlagsHasAScale = 1 << 3,
		TTFCompoundGlyphComponentFlagsObsolete = 1 << 4, // ignore
		TTFCompoundGlyphComponentFlagsMoreComponents = 1 << 5,
		TTFCompoundGlyphComponentFlagsHasXYScale = 1 << 6,
		TTFCompoundGlyphComponentFlagsIsTwoByTwo = 1 << 7,
		TTFCompoundGlyphComponentFlagsHasInstructions = 1 << 8,
		TTFCompoundGlyphComponentFlagsUsesComponentMetric = 1 << 9,
		TTFCompoundGlyphComponentFlagsOverlapCompound = 1 << 10
	};

	enum class TTFCompoundGlyphTransformation
	{
		SameScale, // entry 1
		TwoScales, // entry 2
		FourScales // entry 3
	};

	struct CompoundGlyphData : public GlyphData
	{
		TTFCompoundGlyphComponentFlags flags;
		std::uint16_t glyphIndex;

		union CompoundGlyphArg
		{
			std::uint8_t byte;
			std::uint16_t word;
		};

		CompoundGlyphArg arg1;
		CompoundGlyphArg arg2;

		// some of these variables might not be set/used depending on the flags of the components
		F2Dot14 scale; 
		F2Dot14 xScale; 
		F2Dot14 yScale; 
		F2Dot14 scale01;
		F2Dot14 scale10;

		//TTFCompoundGlyphTransformation transformation;

		CompoundGlyphData* nextGlyph = nullptr;
		
		// only set for "root" glyph data
		std::uint16_t instructionLength;
		std::uint8_t* instructions = nullptr;

		~CompoundGlyphData() 
		{
			delete nextGlyph;
			delete instructions;
		}
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
		GlyphDescription(GlyphDescription&& other) noexcept : data(other.data), 
			numberOfContours(other.numberOfContours), xMin(other.xMin), yMin(other.yMin), 
			xMax(other.xMax), yMax(other.yMax) 
		{
			other.data = nullptr;
		}

		~GlyphDescription() 
		{
			delete data;
		}

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

	// Internal Font Objects //////////////////////////////////////////////

	// for now only supports simple glyph
	class Glyph
	{
	public:
		Glyph() { }

		Glyph(const GlyphDescription& description)
		{
			// temp ///////////////////////////
			//AE_CORE_ASSERT(description.numberOfContours > 0, "Glyph class only supports simple glyphs for now"); 
			///////////////////////////////////

			m_numContours = description.numberOfContours;
			m_outlineMin = Vector2Short(description.xMin, description.yMin);
			m_outlineMax = Vector2Short(description.xMax, description.yMax);

			if (IsSimpleGlyph())
			{
				SimpleGlyphData* data = (SimpleGlyphData*)description.data;
				size_t numPoints = (size_t)data->GetNumPoints();
				m_points.Reserve(numPoints);
				
				// temp
				numFlags = data->numFlags;
				/////////
				
				for (size_t i = 0; i < numPoints; i++)
				{
					TTFOutlineFlags flags = data->GetFlags(i);
					m_points.EmplaceBack(data->GetCoords(i), (flags & TTFOutlineFlagsOnCurve), false);
				}
			}

		}

		bool IsSimpleGlyph() const { return m_numContours > 0; }

		// temp for debug
		void DrawPoints()
		{
			// see for more details: https://learn.microsoft.com/en-us/typography/opentype/spec/ttch01
			static bool printOnCurveOnly = false;
			Vector2 scale = Vector2(0.01f, 0.01f);

			size_t skips = 0;//2;
			size_t endSkip = 0;//14;
			size_t skipCount = 0;

			// should be 56 points for the 'a' char might be some duplicates in that number
			size_t toDraw = m_points.GetCount() - endSkip;

			for (GlyphPoint& point : m_points)
			{
				if (Input::GetKeyDown(KeyCode::T))
				{
					printOnCurveOnly = !printOnCurveOnly;
				}

				if (point.isOnCurve || !printOnCurveOnly)
				{
					if (skipCount >= skips && skipCount < toDraw)
					{
						Renderer::DrawQuad(Vector3(point.coords.x, point.coords.y, 0) * 0.0001f, 0.0f, scale);
					}
				}
				skipCount++;
			}
		}
		////////////////////

	private:
		struct GlyphPoint
		{
			Vector2Int coords;
			bool isOnCurve;
			bool isMidpoint; // might want to remove?

			GlyphPoint() : isOnCurve(false), isMidpoint(false) { }
			GlyphPoint(const Vector2Int& c, bool onCurve, bool midpoint) : coords(c), isOnCurve(onCurve), 
				isMidpoint(midpoint) { }

			bool operator==(const GlyphPoint& other) const
			{
				return isOnCurve == other.isOnCurve && isMidpoint == other.isMidpoint && coords == other.coords;
			}

			bool operator!=(const GlyphPoint& other) const
			{
				return !(*this == other);
			}
		};

		std::int16_t m_numContours;
		Vector2Short m_outlineMin;
		Vector2Short m_outlineMax;
		ADynArr<GlyphPoint> m_points;

		// temp
		size_t numFlags;
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
		file.read((char*)&dir.tag, sizeof(std::uint32_t));

		std::uint32_t data;
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

	IndexToLocationTable ReadLoca(std::ifstream& file, HeaderTable& head, MaximumProfileTable& maxp)
	{
		IndexToLocationTable loca;
		loca.isShort = head.indexToLocFormat == 0;
		loca.arrLen = maxp.numGlyphs + 1;
		if (loca.isShort)
		{
			loca.offsetArr.shortArr = ReadTTFArr<std::uint16_t>(file, loca.arrLen);
		}
		else
		{
			loca.offsetArr.longArr = ReadTTFArr<std::uint32_t>(file, loca.arrLen);
		}

		return loca;
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
		data->numOfContours = numContours;
		data->endPtsOfContours = ReadTTFArr<std::uint16_t>(file, (size_t)numContours); 
		data->instructionLength = ReadTTFVar<std::uint16_t>(file);
		if (data->instructionLength > 0)
		{
			data->instructions = ReadTTFArr<std::uint8_t>(file, (size_t)data->instructionLength);
		}
		else
		{
			data->instructions = nullptr;
		}


		// read flags
		size_t flagIndex = 0;
		size_t numPoints = data->endPtsOfContours[numContours - 1];
		TTFOutlineFlags* tempArr = new TTFOutlineFlags[numPoints];
		std::uint8_t* tempRepeatArr = new std::uint8_t[numPoints];


		for (std::int16_t i = 0; i < numPoints; i++)
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

		SimpleGlyphData::SimpleGlyphCoord* tempCoordArr = new SimpleGlyphData::SimpleGlyphCoord[numPoints];
		size_t coordIndex = 0;

		// read X coords
		for (size_t i = 0; i < numPoints; i++)
		{
			TTFOutlineFlags currFlag = data->GetFlags(i);
			if (currFlag & TTFOutlineXShortVec)
			{
				tempCoordArr[coordIndex].bytes1 = ReadTTFVar<std::uint8_t>(file);
				coordIndex++;
			}
			else if (!(currFlag & TTFOutlineXSameOrPositive))
			{
				//tempCoordArr[coordIndex].bytes2 = ReadTTFVar<std::int16_t>(file);
				//temp
				std::int16_t x = ReadTTFVar<std::int16_t>(file);
				std::int16_t y;
				AssertDataEndianness(&x, &y, sizeof(std::int16_t), Endianness::BigEndian);
				tempCoordArr[coordIndex].bytes2 = y;

				coordinates are not being read correctly but the start of the glyph description is correct 
				so something is wrong with the ReadSimpleGlyphData function
				//
				coordIndex++;
			}
		}

		data->numXCoords = coordIndex;
		data->xCoordinates = new SimpleGlyphData::SimpleGlyphCoord[data->numXCoords];

		for (size_t i = 0; i < data->numXCoords; i++)
		{
			data->xCoordinates[i] = tempCoordArr[i];
		}

		// read y coords
		coordIndex = 0;
		for (size_t i = 0; i < numPoints; i++)
		{
			TTFOutlineFlags currFlag = data->GetFlags(i);
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

		return data;
	}

	CompoundGlyphData* ReadCompoundGlyphData(std::ifstream& file)
	{
		CompoundGlyphData* data = new CompoundGlyphData();
		CompoundGlyphData* currGlyph = data;
		
		do
		{
			currGlyph->flags = ReadTTFVar<TTFCompoundGlyphComponentFlags>(file);
			currGlyph->glyphIndex = ReadTTFVar<std::uint16_t>(file);


			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsArgs1And2AreWords)
			{
				currGlyph->arg1.word = ReadTTFVar<std::uint16_t>(file);
				currGlyph->arg2.word = ReadTTFVar<std::uint16_t>(file);
			}
			else
			{
				currGlyph->arg1.byte = ReadTTFVar<std::uint8_t>(file);
				currGlyph->arg2.byte = ReadTTFVar<std::uint8_t>(file);
			}

			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasAScale)
			{
				currGlyph->scale = ReadTTFVar<F2Dot14>(file);
			}
			else if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasXYScale)
			{
				currGlyph->xScale = ReadTTFVar<F2Dot14>(file);
				currGlyph->yScale = ReadTTFVar<F2Dot14>(file);
			}
			else if (currGlyph->flags & TTFCompoundGlyphComponentFlagsIsTwoByTwo)
			{
				currGlyph->xScale = ReadTTFVar<F2Dot14>(file);
				currGlyph->scale01 = ReadTTFVar<F2Dot14>(file);
				currGlyph->scale10 = ReadTTFVar<F2Dot14>(file);
				currGlyph->yScale = ReadTTFVar<F2Dot14>(file);
			}

			if (currGlyph->flags & TTFCompoundGlyphComponentFlagsMoreComponents)
			{
				CompoundGlyphData* nextGlyph = new CompoundGlyphData();
				currGlyph->nextGlyph = nextGlyph;
				currGlyph = nextGlyph;
			}
		} 
		while (currGlyph->flags & TTFCompoundGlyphComponentFlagsMoreComponents);

		if (currGlyph->flags & TTFCompoundGlyphComponentFlagsHasInstructions)
		{
			data->instructionLength = ReadTTFVar<std::uint16_t>(file);
			data->instructions = ReadTTFArr<std::uint8_t>(file, data->instructionLength);
		}

		return data;
	}

	GlyphDescription ReadGlyphDescription(std::ifstream& file)
	{
		AE_CORE_ASSERT(file.good(), "");

		GlyphDescription glyph;
		
		glyph.numberOfContours = ReadTTFVar<std::int16_t>(file); 
		glyph.xMin = ReadFWord(file);
		glyph.yMin = ReadFWord(file);
		glyph.xMax = ReadFWord(file);
		glyph.yMax = ReadFWord(file);
		
		AE_CORE_ASSERT(file.good(), "");
		if (glyph.numberOfContours > 0)
		{
			glyph.data = ReadSimpleGlyphData(file, glyph.numberOfContours);
			AE_CORE_ASSERT(file.good(), "");
		}
		else if (glyph.numberOfContours < 0)
		{
			glyph.data = ReadCompoundGlyphData(file);
			AE_CORE_ASSERT(file.good(), "");
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
				CompoundGlyphData* compound = (CompoundGlyphData*)g.data;
				int x = 0;
			}
		}
	}

	void PrintLoca(IndexToLocationTable& loca)
	{
		for (size_t i = 0; i < loca.arrLen; i++)
		{
			std::cout << i << ": " << loca.GetGlyphOffset(i) << "\n";
		}
	}

	void PrintContourData(SimpleGlyphData* data)
	{
		for (size_t i = 0; i < data->GetNumPoints(); i++)
		{
			Vector2Int coords = data->GetCoords(i);
			AE_CORE_INFO("%d, %d", coords.x, coords.y);
		}
	}

	///////////////////////////////////////


	// temp font object used for debugging

	void DebugTTFFont::DebugDrawGlyph()
	{
		// displaying points of 'a'
		glyphs[68].DrawPoints();
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

		HeaderTable head;
		HorizontalHeader hhea;
		ADynArr<LongHorizontalMetric> hmtx;
		ADynArr<Glyph> glyf;
		MaximumProfileTable maxp;
		Cmap cmap;
		IndexToLocationTable loca;

		TableDirectory* locaDir = nullptr;
		ASinglyLinkedList<TableDirectory> tableDirectories;
		ASinglyLinkedList<TableDirectory*> dependencyTables; // tables required by other tables for initialization

		for (std::uint16_t i = 0; i < offsetSubtable.numTables; i++)
		{
			tableDirectories.Add(ReadTableDir(file));
			TableDirectory* currDir = &tableDirectories[0];

			// temp ///////////////////////////////////////
			char tableID[5];
			memcpy(tableID, (std::uint32_t*)&currDir->tag, 4);
			tableID[4] = '\0';
			///////////////////////////////////////////////
			switch(currDir->tag)
			{
			case s_hheaTag:
			case s_maxpTag:
			case s_headTag:
				dependencyTables.Add(currDir);
				break;

			case s_locaTag:
				locaDir = currDir;
				break;
			}
		}

		for (TableDirectory* dir : dependencyTables)
		{
			size_t oldPos = file.tellg();
			file.seekg(dir->offset);
			switch(dir->tag)
			{
			case s_headTag:
				head = ReadHeaderTable(file);
				break;

			case s_hheaTag:
				hhea = ReadHorizontalHeader(file);
				break;

			case s_maxpTag:
				maxp = ReadMaximumProfileTable(file);
				break;
			}
			file.seekg(oldPos);
		}

		{
			AE_CORE_ASSERT(locaDir != nullptr, "");
			size_t oldPos = file.tellg();
			file.seekg(locaDir->offset);
			loca = ReadLoca(file, head, maxp); 
			file.seekg(oldPos);
		}


		for (TableDirectory& dir : tableDirectories)
		{
			if (FindTable(dependencyTables, dir.tag) != nullptr)
			{
				continue;
			}
			file.seekg(dir.offset);

			switch(dir.tag)
			{
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
					file.seekg(loca.GetGlyphOffset(i) + dir.offset);
					
					// temp
					if (glyf.GetCount() == 68)
					{ 
						/*
						contour data is way off coordinate wise
						double check that the coordinates are being read properly, next thing will be 
						to check that the loca table is being used to find the index of a glyph properly
						*/

						size_t pos = file.tellg();
						GlyphDescription des = ReadGlyphDescription(file);
						//PrintContourData((SimpleGlyphData*)des.data);
						Vector2Int coords = ((SimpleGlyphData*)des.data)->GetCoords(3) 
							- ((SimpleGlyphData*)des.data)->GetCoords(2);
						file.seekg(pos);
					}
					//reading glyph description incorrectly (use the usual website to verify the data of the glyphs see first tab)
					//
					glyf.Add(ReadGlyphDescription(file));
				}
				break;
			}
			AE_CORE_ASSERT(file.good(), "");

			//look into drawing glyphs from the glyf data:
			//https://docs.microsoft.com/en-us/typography/opentype/spec/ttch01

		}

		// temp
		AReference<DebugTTFFont> tempFont = AReference<DebugTTFFont>::Create();
		tempFont->glyphs = std::move(glyf);
		return tempFont;
	}

}